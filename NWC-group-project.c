/******************************************************************************
<Author Name>
Created Febraury 23, 2025

NWC Complete MAIN UI
- Account Registration
- Account Login

//HARD CODED ADMIN LOGIN
Password "00000000"
Email "admin@gmail.com"

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wchar.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
//#include <termios.h>


//constants for text cosmetics
#define bold_start "\e[1m"
#define bold_end  "\e[m"
#define underline_start "\033[4m"
#define underline_end "\033[0m"

//constants for program
#define max_length 50
#define max_e_length 512
#define password_length 31
#define starter_text "LOGIN DATABASE"
#define file_open_error "-Error opening file-"
#define data_breakpoint "\n;"
#define max_ID 99999999
#define min_ID 10000000

//Keyboard ASCII map
#define backspace_key 8
#define enter_key 13
#define tab_key 9
#define delete_key 127
#define space_key 32

//Initalizing variables
char registered;
char data_to_check[max_length];
char terminal;
char clear_terminal[5 +1];
char hashed_password[50];
char tempID_hold[max_length] = {0};

// set to 0 to see function outputs
// set to 1 to not see function outputs
int debug = 1;
int debug_scanfpassword = 1;

//Initializing file name/s
char *loginfile = "login_database.txt";
char *customerdatabase = "customer_database.txt";
char *auditfile = "audit_logs.txt";

char admin_password[] = "00000000";
char admin_email[] = "admin@gmail.com";
/***************************** FUNCTIONS **************************/

//Prototypes
/**1. Function that checks if a file under the stated
name, already exists**/
int does_file_exist (const char *filename);


/**2. Function that checks if data entered already
exists on a given file**/

// 0 if the file could not be opened
//1 if duplicate data was found
//2 if no duplicate data was found
int duplicate_check (char *data_to_check, char *filename);

/**3. Function that appends data from customer to 
login_database.txt**/
int addcustomer (char *filename, char *filename2, char *customerID,
                    char *user_firstname, char *user_lastname,
                    char *user_email,char *user_password);

/**4. Function that takes a users email and password. These
inputs are then checked across a database and a matched found**/
// Returns 1 if login unsuccessful
// Returns 0 if login successful
// Returns 3 if login sucessful for ADMIN account
int user_login (char *email, char *password, char* tempID);

/**5. Function --NOT USED--
int getch();**/

/**6. Function generates a random ID given length variable
which determines how long the ID should be**/
int generateID ();

/**7. Function that takes input from user and masks the characters 
given with an "*" **/
void scanfpassword (char* string_input);

/**8. Function that sets all characters in an array to lowercase**/
void setall_lowercase (char* string);

/**9. Function that takes a string as input and uses a modified DJB2
hashing algorithm**/
void hash_djb2(char *password, char *hash_out);

/**10. Function that logs account creation**/
void auditaddcustomer(int audit_type, char *customerID);

/**11. Function that logs account login**/
void auditlogin(int audit_type, char *customerID);
/*******************************************************************/

enum income_class
{
    Low = 125,//(daily usage up to 125 L)
    Low_Medium = 175, //(daily usage up to 175 L)
    Medium = 220, //(daily usage up to 220 L)
    Medium_High = 250, //(daily usage up to 250 L)
    High = 300, //(daily usage up to 300 L)
};


//Structure for all customer data
struct customer
{
    char customerID[9];
    char firstname[max_length];
    char lastname[max_length];
    char email[max_length];
    char password[password_length + 1];
    int income_class;
    
    char premisesID[9];
};

struct audit
{
    char date[max_length];
    char time[max_length];   
};

int main()
{
  
  //Checks what terminal the user is running so the clear command line function "system()"" works properly
    printf("-Are you on a windows or linux terminal -\n(L) for Linux\n(W) for Windows - Select this if unsure\n");
    scanf(" %c", &terminal);
    
    if(terminal == 'W' || terminal == 'w')
    {
        strcpy(clear_terminal,"cls");
    }
    else if (terminal == 'L' || terminal == 'l')
    {
        strcpy(clear_terminal,"clear");
    }
    
    
/************************* DATABASE CREATION **********************/
    
    system(clear_terminal); //Clears command line UI
    // file pointer
    FILE*fp;
    
    //CREATING LOGIN DATABASE
    //Checking if file exists, and creating login database if not.
    if (does_file_exist(loginfile) == 0)
    {
        // creating file using fopen() access mode "w"
        fp = fopen(loginfile, "w");
        
        if(fp == NULL)
        {
            printf("\n-%s-\n",file_open_error);
        }
            
        printf("-Login database has been created-");
        fclose(fp);
            
        //Appening title to Database 
        //( functions later in program dosent work if there isnt something in the .txt file )
        fp = fopen(loginfile, "a");
        const char *text = "LOGIN DATABASE\n\nUserID: 10101010\nUser Email: admin@gmail.com\nHashed Password: BB33D8B\n;\n\n";
        fputs(text, fp);
            
        printf(" starter data inserted-");        
        fclose(fp);
        
    }
    else
    {
        printf("-Login Database found ( new file not created )-");
    }

    //CREATING CUSTOMER DATABASE
    //Checking if file exists, and creating customer database if not.
    if (does_file_exist(customerdatabase) == 0)
    {
        // creating file using fopen() access mode "w"
        fp = fopen(customerdatabase, "w");
        
        if(fp == NULL)
        {
            printf("\n-%s-\n",file_open_error);
        }
            
        printf("\n-Customer database has been created-");
        fclose(fp);
            
        fp = fopen(customerdatabase, "a");
        const char *text = "CUSTOMER DATABASE\n\n";
        fputs(text, fp);
        
        fclose(fp);
        printf(" starter data inserted-");
        
    }
    else
    {
        printf("\n-Customer Database found ( new file not created )-");
    }

    //CREATING AUDIT LOGS
    //Checking if file exists, and creating customer database if not.
    if (does_file_exist(auditfile) == 0)
    {
        // creating file using fopen() access mode "w"
        fp = fopen(auditfile, "w");
        
        if(fp == NULL)
        {
            printf("\n-%s-\n",file_open_error);
        }
            
        printf("\n-Audit Logs have been created-");
        fclose(fp);
            
        fp = fopen(auditfile, "a");
        const char *text = "AUDIT LOGS\n\n";
        fputs(text, fp);
        
        fclose(fp);
        printf(" starter data inserted-");
        
    }
    else
    {
        printf("\n-Audit Logs found ( new file not created )-");
    }
    
    
/***************************** MAIN UI **************************/
    
    //Are you a registered user?
    printf("\n\nAre you a register user? \n'Y' for yes\n'N' for no\n\n");
    scanf(" %c", &registered);
    
    //structure variable <user>
    struct customer user;
    
    //CREATING NEW ACCOUNT
    if(registered=='N' || registered=='n')
    {
        
        
        system(clear_terminal); //Clears command line UI

        //Outputting header
        printf(bold_start "\n---ACCOUNT REGISTRATION---\n\n" bold_end);
        
        
        printf(underline_start"Enter an email:\n"underline_end);
        scanf("%s", user.email);
        setall_lowercase(user.email);
        
        //Validating email format by checking if the 3 most popular email domain names are included
        while(strstr(user.email,"@gmail.com") == NULL &&
              strstr(user.email,"@yahoo.com") == NULL &&
              strstr(user.email,"@hotmail.com") == NULL) //@<domain_name>.com was not found do:
        {
            printf("\n-Incorrect email format-\n\nAccepted domains:\n- @gmail.com\n- @yahoo.com\n- @hotmail.com\n\n");
            printf(underline_start"Enter an email:\n"underline_end);
            scanf("%s", user.email);
            setall_lowercase(user.email);
        }

        /**Checks if the email entered is already being used and
        asks the user to enter a new one if so**/
        //On confirmation that email is available, user is prompted to enter password and their names.
        if(duplicate_check(user.email,loginfile) != 0)
        {
            
            while(duplicate_check(user.email,loginfile) != 2)
            {
                printf("\n-Email already in use!-\n\n");
                printf(underline_start"Enter new email:\n"underline_end);
                scanf("%s", user.email);
                setall_lowercase(user.email);
            }
            

            strcpy(user.password,"temp");//Placeholder value so checks after this point works
            
            //Gets password from user
            while(strlen(user.password) < 8)
            {
                printf(underline_start"\nEnter your password \n( %d characters max ):\n"underline_end,password_length);
                scanf(" %s",user.password);
                
                if(strlen(user.password) < 8)
                {
                    printf("\nPassword should be AT LEAST 8 characters long\n");
                }
            }

            printf(underline_start"\nEnter your first name:\n"underline_end);
            scanf("%s", user.firstname);
            
            printf(underline_start"\nEnter your last name:\n"underline_end);
            scanf("%s", user.lastname);
            
            
            //Generating random customer ID
            int temp_ID = generateID();
            
            //Converts variable "temp_ID" ^ to string and puts it in variable "customerID"
            sprintf(user.customerID, "%d", temp_ID);
            
            //Verifying the ID isnt already in use (just incase)
            while(duplicate_check(user.customerID,customerdatabase) != 2)
            {
                temp_ID = generateID();
                
                //Converts variable "a" ^ to string and puts it in variable "customerID"
                sprintf(user.customerID, "%d", temp_ID);
            }
            
            //Inserting Data into their respective files
            int fault_check = addcustomer(loginfile,customerdatabase,user.customerID,
                            user.firstname,user.lastname,user.email,
                            user.password);

            auditaddcustomer(fault_check,user.customerID);
            

            system(clear_terminal); //Clears command line UI
            
            
            printf(underline_start"Would you like to LOGIN? \n"underline_end"'Y' for yes\n'N' for no\n\n");
            scanf(" %c", &registered);
        }
        else
        {
            printf("\n-Login database could not be accessed-");
        }
    }

    
    //Existing account LOGIN
    int ch;
    int i = 0;
    int attempt = 2;
    int total_attempts = 6;
            
    if(registered == 'Y' || registered =='y')
    {
        
        system(clear_terminal); //Clears command line UI

        
        printf(bold_start "---ACCOUNT LOGIN---\n\n" bold_end);
        
        printf(underline_start"Enter your email:\n"underline_end);
        scanf(" %s", user.email);
        setall_lowercase(user.email);
        
        printf(underline_start"\nEnter your password:\n"underline_end);
        
        if(debug_scanfpassword != 1)
        {
            //DEBUG
            scanf(" %s", user.password);
        }
        else
        {
            //Gets password from user while masking input
            scanfpassword(user.password);            
        }
            
         
        //Verification check for password and email
        int login_access = user_login(user.email,user.password,tempID_hold);
        
        while (login_access != 0 && login_access != 2 && attempt < total_attempts)
        {
            system(clear_terminal); //Clears command line UI
            printf(bold_start "\n---ACCOUNT LOGIN---\n" bold_end);
            
            printf("\n-Email or Password is incorrect - ");
            printf("attempt #: %d - you have %d attempts left -\n",attempt,total_attempts-attempt);
            
            printf(underline_start"\nEnter your email:\n"underline_end);
            scanf(" %s", user.email);
            setall_lowercase(user.email);
            
            printf(underline_start"\nEnter your password:\n"underline_end);

            if(debug_scanfpassword != 1)
            {
                //DEBUG
                scanf(" %s", user.password);
            }
            else
            {
                //Gets password from user while masking input
                scanfpassword(user.password);            
            }
                
            //Checking credentials over
            login_access = user_login(user.email,user.password,tempID_hold);

            attempt++;
        }

        //Check this variable when determining if the user should be given access to the program beyond this point
        if(login_access == 0)
        {
            printf("\n\nlogin successful\n\n");
            auditlogin(login_access,tempID_hold);

        }
        else if (login_access == 2)
        {
            printf("\n\nlogin successful - ADMIN USER -\n\n");
            auditlogin(login_access,tempID_hold);
        }
    }
    //Terminate program
    else if(registered == 'N' || registered == 'n')
    {
        printf(bold_start"\n-Application Closed-"bold_end);
    }
    
    return 0;
}


/************************* FUNCTION DEFINITION **********************/

//Definition of function 1.
//  0 = File doesn't exist
//  1 = File exists
int does_file_exist (const char *filename)
{
    //Attempts to open file with name stored in "filename" in READ (r) mode
    FILE*fp = fopen(filename, "r");
    
    if (fp == NULL)
    {
        return 0;
    }
    fclose(fp);
    return 1;
}

//Definition of function 2.
//  0 if the file could not be opened
//  1 if duplicate data was found
//  2 if no duplicate data was found
int duplicate_check (char *data_to_check,char *filename)
{
    //Attempts to open file with name stored in "filename" in READ (r) mode
    FILE*fp = fopen(filename, "r");
    char str[max_e_length];
    char *exist;
    int killstop = 1, func_validation = 0;

    if(fp != NULL)
    {
        while(fgets(str, max_e_length, fp)!=NULL)
        {
            exist = strstr(str,data_to_check);
            
            if(exist != NULL && killstop != 0)
            {
                func_validation = 1; // Duplicate data was found
                killstop = 0;
            }
            else if (exist == NULL && killstop == 1)
            {
                func_validation = 2; // Duplicate data was not found
            }
        }
    }
    else
    {

    }

    return func_validation;
}

//Definition of function 3.
//  Return 0 if function executed successfully
//  Return 1 if function was unable to access a given file
int addcustomer (char *filename,char *filename2, char *customerID, char *user_firstname, char *user_lastname,char *user_email,char *user_password)
{
    FILE *fp;
    char tmp_user_password[9];
            
    //attempts to open login Database file
    fp = fopen(filename, "a");
    if(fp != NULL)
    {
        //appends data passed through into file opened previously ^
        fputs("UserID: ", fp);
        fputs(customerID, fp);
        fputs("\n", fp);
                
        fputs("User Email: ", fp);
        fputs(user_email, fp);
        fputs("\n", fp);
                
        fputs("Hashed Password: ", fp);
        
        //Hashing password & storing said hash in password variable
        hash_djb2(user_password,hashed_password);
        
        fputs(hashed_password, fp);
        fputs(data_breakpoint, fp);
        fputs("\n\n", fp);
    }
    else
    {
        return 1;
    }
    
    fclose(fp);
        
    //attempts to open Customer Database file
    fp = fopen(filename2, "a");
    if(fp != NULL)
    {
        //appends data passed through into file opened previously ^
        fputs("UserID: ", fp);
        fputs(customerID, fp);
        fputs("\n", fp);
            
        fputs("User First Name: ", fp);
        fputs(user_firstname, fp);
        fputs("\n", fp);
        
        fputs("User Last Name: ", fp);
        fputs(user_lastname, fp);
        fputs(data_breakpoint, fp);
        fputs("\n\n", fp);
    }
    else
    {
        return 1;
    }
    
    fclose(fp);
    
    return 0;
}


//Definition of function 4.
// Return 1 if credentials dont match
// Return 0 if credentials match

/**Function finds the matching email record in loginfile and then
goes down by one line, and retrieves the password. These are then
checked against the user inputted data.
**/
// Returns 0 if login successful
// Returns 1 if login unsuccessful
// Returns 2 if login sucessful for ADMIN account
int user_login (char *email, char *password, char* tempID)
{
    //1. Attempts to open file name stored in "loginfile" ( login_database.txt)
    FILE*fp = fopen(loginfile, "r");

    //Hashing password & storing said hash in second password variable
    hash_djb2(password,hashed_password);

    //Intiliazing variables
    char str[max_e_length];
    char temp_email[max_length] = {0};
    char temp_password[max_length]= {0};
    char tmpID_hold[max_length] = {0};
    int login_validation = 1, a;
    int debug_a;
    long int position1;

    if (fp != NULL)
    {
        while(fgets(str,max_e_length,fp) != NULL)
        
        {
            if(strstr(str,"UserID: ") != NULL && login_validation == 1)//An id prefix FOUND
            {
                position1 = ftell(fp); // Saves where ID was found as character index
                
                strcpy(tmpID_hold,str);
                //Sanitizing values ( removing new line values, carriages, white space and other unwanted characters)
                tmpID_hold[strcspn(tmpID_hold, "\r\n")] = '\0';

                if(debug == 0)
                {
                    printf("\n\nFound: %s",tmpID_hold);
                    printf("\nuserID pointer location: %ld\n", position1);
                }
            }
            
            if(strstr(str, "User Email: ") != NULL && strstr(str, email) != NULL)//If email is found
            {
                //Assigns email found at if statement to found_email
                char *found_email = strstr(str, email);
                
                if (debug == 0)
                {
                    printf("\ndebug - found email: %s",found_email);
                    a = strlen(found_email);
                    printf("strlen: %d", a);
                }
                
                if (found_email != NULL)
                {
                    strcpy(temp_email, found_email);
                    
                    if (debug == 0)
                    {
                        printf("\ndebug - temp email: %s",temp_email);
                        a = strlen(temp_email);
                        printf("strlen: %d", a);
                    }
                    
                    //Sanitizing values ( removing new line values, carriages and white space)
                    temp_email[strcspn(temp_email, "\r\n")] = '\0';
                    
                    if (debug == 0)
                    {
                        printf("\ndebug - temp email after sanitization: %s\n",temp_email);
                        a = strlen(temp_email);
                        printf("strlen: %d\n", a);
                    }
                }

                if(fgets(str,max_e_length,fp)!= NULL)// Goes down by one line (Password should be stored here)
                {
                    if(strstr(str, hashed_password)!= NULL)//If password is found
                    {
                        //Assigns password found at if statement to found_pswd
                        char *found_pswd = strstr(str, hashed_password);
                        if (found_pswd != NULL)
                        {
                            strcpy(temp_password, found_pswd);
        
                            //Sanitizing values ( removing new line values, carriages and white space)
                            temp_password[strcspn(temp_password, "\r\n")] = '\0';

                        }
                    }

                    /*DEBUGGING*/
                    if (debug == 0)
                    {
                        printf("\nlogin function email(from file): %s",temp_email);
                        printf("\nlogin function email(from user): %s",email);
                        if(strlen(temp_password)!=0)
                        {
                            printf("\nlogin function pswd(from file): %s",temp_password);
                        }
                        else
                        {
                            printf("\nlogin function pswd(from file): NO MATCH");
                        }
                        printf("\nlogin function pswd(from user): %s",hashed_password);

                        if(strcmp(temp_email,admin_email)==0)
                        {
                            debug_a = 0;
                        }
                        else
                        {
                            debug_a = 1;
                        }
                        printf("\n\nstrcmp admin email: %d",debug_a);
                        
                        if(strcmp(temp_email,email)==0)
                        {
                            debug_a = 0;
                        }
                        else
                        {
                            debug_a = 1;
                        }
                        printf("\nstrcmp email: %d",debug_a);
                        
                        if(strcmp(temp_password,hashed_password)==0)
                        {
                            debug_a = 0;
                        }
                        else
                        {
                            debug_a = 1;
                        }
                        printf("\nstrcmp password: %d",debug_a);

                    }
                
                }
                
                if(strcmp(temp_email,admin_email) == 0 && strcmp(temp_password,hashed_password) == 0)
                {
                    login_validation = 2; //Successful admin login
                }
                else if(strcmp(temp_email,email) == 0 && strcmp(temp_password,hashed_password) == 0)
                {
                    login_validation = 0; //Successful login
                }
            }
        }
    }
    else
    {
        printf("\n-Could not access login database-\n");
    }

    /**Given the login was successful, goes back to place in file
    where ID was found and saves that value into variable passed into function
    **/
    if(login_validation == 0 || login_validation == 2)
    {
        if(fseek(fp,position1,SEEK_CUR)==0)//Brings file pointer 1 line before UserID associated with user credentials
                                           //if statement only continues if fseek was successful in moving pointer (returned 0)
        {
            if(fgets(str,max_e_length,fp) != NULL)//Goes down by one line and stores string found in "str"
            {
                strcpy(tmpID_hold,str);
            }

            //Sanitizing string ( removing new line values(\n), carriages(\r), white space(" ") and other unwanted characters)
            if(strlen(tmpID_hold)>8)
            {
                char *b = tmpID_hold;
                b += strlen("UserID: "); // Removes all characters before actual ID string
                strcpy(tmpID_hold,b);
                tmpID_hold[strcspn(tmpID_hold, "\r\n")] = '\0';
            }

            strcpy(tempID,tmpID_hold);

            if (debug == 0)
            {
                printf("\n\nID passed through: %s\n",tempID);
            }
        }
        else if(fseek(fp,position1,SEEK_CUR) !=0 && debug == 0)
        {
            printf("\nfseek aint working\n");
        }
    }

    fclose(fp);
    return login_validation;
}

/**Definition of function 5.
int getch()
{
    int ch;
    // struct to hold the terminal settings
    struct termios old_settings, new_settings;
    // take default setting in old_settings
    tcgetattr(STDIN_FILENO, &old_settings);
    // make of copy of it (Read my previous blog to know 
    // more about how to copy struct)
    new_settings = old_settings;
    // change the settings for by disabling ECHO mode
    // read man page of termios.h for more settings info
    new_settings.c_lflag &= ~(ICANON | ECHO);
    // apply these new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    // now take the input in this mode
    ch = getchar();
    // reset back to default settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    return ch;   
}**/

//Definition of function 6.
int generateID ()
{
    srand(time(NULL));
    // Generate random number between max ID value (99999999) and min ID value (10000000)
    long long ID = ((long long)rand()*rand())%((max_ID-min_ID)+1);

    //DEBUG
    //printf("\n\nFUNCTION ID TEST: %lld\n\n", ID);
    return ID;
}

//Definition of function 7.
void scanfpassword (char* string_input)
{
    char ch;  //Referenced by password masking
    int charposition = 0;  //Referenced by password masking

    //MASK password INPUT ( method from "LearningLad" on youtube)
    while(1)
    {
        ch = _getwch();
        if(ch == enter_key) //When user presses enter key
        {
            break;
        }
        else if (ch == backspace_key||ch == delete_key) //When user presses backspace
        {
            if(charposition > 0)
            {
                charposition--; //moves char index back by 1
                string_input[charposition] = '\0'; //Removes value found in index ^
                printf("\b \b"); //Brings cursor back to current index
            }
            
        }
        else if (ch == tab_key || ch == space_key) //When user pressed space/tab it ignores said input
        {
            continue;   
        }
        else //When user enters "normal" characters
        {
            if (charposition < password_length)
            {
                string_input[charposition] = ch;
                charposition++;
                printf("*");
            }
            else
            {
                printf("\nMAX password length exceeded. Preceding characters saved, following characters discarded");
                break;
            }
        }
                
    }
    string_input[charposition] = '\0';
}

//Defintion of function 8.
void setall_lowercase (char* string)
{
    char temp_string[max_e_length] = {0};
    int i;

    strcpy(temp_string, string);

    for(i = 0; i<strlen(temp_string); i++)
    {
        temp_string[i] = tolower(temp_string[i]);
    }
    temp_string[i] = '\0';

    strcpy(string, temp_string);
}

//Defintion of function 9.
void hash_djb2(char *password, char *hash_out)
{

    //DJB2 Hashing Algorithm
    unsigned long hash = 5381;
    unsigned long prime = 585105623;
    char temp_hash[10];
    
    int ch;

    while ((ch = *password++))
    {
        hash = ((hash << 5) + hash) +ch;
        
        hash = hash%prime;//Preventing overflow (number getting too big)
    }
    
    sprintf(temp_hash,"%lu",hash);
    int slen = strlen(temp_hash);
    
    if(slen<9)
    {
        hash = hash * pow(10,9-slen); //Padding hash integer such that its always 9 digits long
    }
    
    //Converting decimal to Hexadecimal (Method by "AllTech" on Youtube)
	char reversedDigits[100];
	int i = 0;
	
	while(hash > 0)
	{
		int remain = hash % 16;
		
		if(remain < 10)
			reversedDigits[i] = '0' + remain;
		else
			reversedDigits[i] = 'A' + (remain - 10);
		
		hash = hash / 16;
		i++;
	}

    //Assigns last value in char array a terminator
    reversedDigits[i] = '\0';
	
	int size = i;
	
    //Flips elements in array
    for(i = 0; i < size/2; i++)
    {
        // 1.Finds the index the current value will be swapped with
        int move_index = (size-1)-i;
        
        // 2.Saves the value found at the previous ^ index
        char tmp = reversedDigits[move_index];
        
        
        /** 3.Copies the value from the the current index(i)
        into the opposite index found at 1. **/
        reversedDigits[move_index] = reversedDigits[i];
        
        
        /** 4.Takes the value stored in 2. and copies it into the 
        current index (i) **/
        reversedDigits[i]=tmp;
    }
    strcpy(hash_out,reversedDigits);

}

//Defintion of function 10.

void auditaddcustomer(int audit_type, char *customerID)
{
    //AUDITING
    FILE *fp;
    fp = fopen(auditfile,"a"); //attempts to open audit file
            
    struct audit log;

    if(fp != NULL)
    {
        //Gets current time (Method by "CodeVault" on youtube)
        time_t t = time(NULL);
        struct tm date = *localtime(&t);
                


        sprintf(log.date,"%02d/%02d/%d\n", date.tm_mon +1 , date.tm_mday, date.tm_year + 1900);
        sprintf(log.time,"%d:%02d", date.tm_hour, date.tm_min);

        if (audit_type == 0) //Customer was added successfully
        {
            fputs("Account created (by customer)\n", fp);
                    
            //Format MONTH - DAY - YEAR
            fputs("Date: ", fp);
            fputs(log.date, fp);

            //Format HOUR - MINUTE
            fputs("Time: ", fp);
            fputs(log.time, fp);
            fputs("\n", fp);

            fputs("New account ID: ", fp);
            fputs(customerID, fp);

            fputs(data_breakpoint, fp);
            fputs("\n\n", fp);
        }
        else if (audit_type == 1) //Attempt to add a customer failed
        {
            fputs("Failed to create account (by customer)\n", fp);
                    
            //Format MONTH - DAY - YEAR
                fputs("Date: ", fp);
                fputs(log.date, fp);

            //Format HOUR - MINUTE
            fputs("Time: ", fp);
            fputs(log.time, fp);
                    
            fputs(data_breakpoint, fp);
            fputs("\n", fp);
        }
    }
    else
    {
        printf(file_open_error);
    }

    fclose(fp);
}

void auditlogin(int audit_type, char *customerID)
{
    //AUDITING
    FILE *fp;
    fp = fopen(auditfile,"a"); //attempts to open audit file
            
    struct audit log;

    if(fp != NULL)
    {
        //Gets current time (Method by "CodeVault" on youtube)
        time_t t = time(NULL);
        struct tm date = *localtime(&t);
                
        sprintf(log.date,"%02d/%02d/%d\n", date.tm_mon +1 , date.tm_mday, date.tm_year + 1900);
        sprintf(log.time,"%d:%02d", date.tm_hour, date.tm_min);

        if (audit_type == 0) //User logged in successfully
        {
            fputs("Account login (by customer)\n", fp);
                    
            //Format MONTH - DAY - YEAR
            fputs("Date: ", fp);
            fputs(log.date, fp);

            //Format HOUR - MINUTE
            fputs("Time: ", fp);
            fputs(log.time, fp);
            fputs("\n", fp);

            fputs("ID of account login: ", fp);
            fputs(customerID, fp);

            fputs(data_breakpoint, fp);
            fputs("\n\n", fp);
        }
        else if (audit_type == 2) //ADMIN user logged in successfully
        {
            fputs("Account login (by admin)\n", fp);
                    
            //Format MONTH - DAY - YEAR
            fputs("Date: ", fp);
            fputs(log.date, fp);

            //Format HOUR - MINUTE
            fputs("Time: ", fp);
            fputs(log.time, fp);
            fputs("\n", fp);

            fputs("ID of account login: ", fp);
            fputs(customerID, fp);

            fputs(data_breakpoint, fp);
            fputs("\n\n", fp);
        }
    }
    else
    {
        printf(file_open_error);
    }

    fclose(fp);
}
