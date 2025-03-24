/******************************************************************************
<Author Name>
Created Febraury 23, 2025

//If using linux, change "_getwch()" to "getch()"

\\WORKING ON EDIT CUSTOMER

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
#include <errno.h>

//constants for text cosmetics
#define bold_start "\e[1m"
#define bold_end  "\e[m"
#define underline_start "\033[4m"
#define underline_end "\033[0m"
#define text_red_start "\033[0;31m"
#define text_color_end "\033[0m"

//constants for program
#define max_length 50
#define max_e_length 1024
#define password_length 31
#define starter_text "LOGIN DATABASE"
#define file_open_error "-Error opening file-"
#define data_breakpoint ";"
#define max_ID 9999999
#define min_ID 1000000

//Keyboard ASCII map
#define backspace_key 8
#define enter_key 13
#define tab_key 9
#define delete_key 127
#define space_key 32

//Database prefix
#define id_prefix "UserID: "
#define fname_prefix "User First Name: "
#define lname_prefix "User Last Name: "
#define email_prefix "User Email: "
#define password_prefix "Hashed Password: "
#define premisesid_prefix "Premises ID #"
#define metersize_prefix "Meter Size #"
#define lastmeter_r_prefix "Last Meter Reading #"

//Initalizing variables
char registered;
char data_to_check[max_length];
char terminal;
char clear_terminal[5 +1];
char hashed_password[50];
char tempID_hold[max_length] = {0};
int delay_time = 2;
float print_delay = .05;

// set to 0 to see function outputs
// set to 1 to not see function outputs
int debug = 1;
int debug_scanfpassword = 1; //set to 0 if using linux terminal (wip)

//Initializing file name/s
char *loginfile = "login_database.txt";
char *customerdatabase = "customer_database.txt";
char *auditfile = "audit_logs.txt";

char admin_email[] = "admin@gmail.com";
char admin_password[] = "00000000";
/***************************** FUNCTIONS **************************/

//Prototypes
/**1. Function that checks if a file under the stated
name, already exists**/
int does_file_exist (const char *filename);


/**2. Function that checks if data entered already
exists on a given file. A prefix is used so that the function looks for the correct
data.**/

// 0 if the file could not be opened
// 1 if duplicate data was found
// 2 if no duplicate data was found
int duplicate_check (char *data_to_check,char *filename,char *prefix_to_check);

/**3. Function that appends data from customer to 
login_database.txt**/
int addcustomer (char *filename, char *filename2, char *customerID,
                    char *user_firstname, char *user_lastname,
                    char *user_email,char *user_password,int add_type);

/**4. Function that takes a users email and password. These
inputs are then checked across a database and a matched found**/
// Returns 1 if login unsuccessful
// Returns 0 if login successful
// Returns 2 if login sucessful for admin account
int user_login (char *email, char *password, char* tempID);

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
// 0 For account created by customer
// 1 For account created by admin
void auditaddcustomer(int audit_type, char *customerID);

/**11. Function that logs account login**/
void auditlogin(int audit_type, char *customerID);

void audit_editcustomer(int audit_type, char * customerID);

/**12. Function delays next line execution by int passed
through in seconds ( Method from https://stackoverflow.com/questions/3930363/implement-time-delay-in-c )**/
void delay_cpu(float delay);

/*13. Function*/
void close_console(int delay_time_seconds, float print_t_delay);

/*14. Function that removes unwanted characters from strings such as "\n" and "\r"*/
// Set sanitize type to 0 to only remove \n 
// Set sanitize type to 1 to remove \n and \r
void strsanitize(char *input_string,const int sanitize_type);

/*15. Function that takes in a string pointer and clears the contents of said string array*/
void clear_stringarray(char *string_tobe_cleared);

/*16. Function that takes in the file to be edited, where in the file the edit must take place, and
what must be put in said place as a replacement*/
// Returns 1 if unsuccessful edit was made
// Returns 0 if successful edit was made
//int edit_record(const char *file_to_open,long unsigned position_in_file_edit,char *replacement_string);

//Terminals
void admin_terminal(char *terminal_clear_string);

void customer_terminal(char *terminal_clear_string);

void remove_prefix(char *string_to_edit, char *prefix_to_remove);
/*******************************************************************/

enum meter_size
{
    meter1 = 15, //15 millimeter
    meter2 = 30, //30 millimeter
    meter3 = 150, //150 millimeter
};

enum income_class
{
    Low = 125, //(daily usage up to 125 L)
    Low_Medium = 175, //(daily usage up to 175 L)
    Medium = 220, //(daily usage up to 220 L)
    Medium_High = 250, //(daily usage up to 250 L)
    High = 300, //(daily usage up to 300 L)
};

struct audit
{
    char date[max_length];
    char time[max_length];   
};

int main()
{
  
  //Checks what terminal the user is running so the clear command line function "system()" works properly
    printf("-Are you on a windows or linux terminal -\n(L) for Linux\n(W) for Windows - Select this if unsure\n");
    scanf(" %c", &terminal);
    
    if(terminal == 'W' || terminal == 'w')
    {
        strcpy(clear_terminal,"cls");
    }
    else if (terminal == 'L' || terminal == 'l')
    {
        strcpy(clear_terminal,"clear");
        debug_scanfpassword = 0;
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
        //( Inserting Starter data into txt file )
        fp = fopen(loginfile, "a");
            char text[max_e_length];


            char temp_password_hold_initilization[max_length];

            //Hashing admin password before storing it in file
            hash_djb2(admin_password,temp_password_hold_initilization);

            //Formats and stores starter data in variable "text"
            snprintf(text,max_e_length,"LOGIN DATABASE\n\nUserID: 1010101\nUser Email: %s\nHashed Password: %s\n;\n\n",admin_email,temp_password_hold_initilization);
            
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

//Structure for all customer data (required for login and registration)
    typedef struct 
    {
        char customerID[9];
        char firstname[max_length];
        char lastname[max_length];
        char email[max_length];
        char password[password_length + 1];
    }customer;
    
    //Are you a registered user?
    printf("\n\nAre you a registered user? \n(Y) for yes\n(N) for no\n\n");
    scanf(" %c", &registered);
    
    while(registered!='N'&&registered!='N'&&registered!='Y'&&registered!='y')
    {
        fflush(stdin);
        system(clear_terminal);
        printf(bold_start"\n\n-Invalid input-"bold_end);
        printf("\n\nAre you a registered user? \n(Y) for yes\n(N) for no\n\n");
        scanf(" %c", &registered);
    }
    
    //structure variable <user>
    customer user;
    
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
        if(duplicate_check(user.email,loginfile,email_prefix) != 0)
        {
            
            while(duplicate_check(user.email,loginfile,email_prefix) != 2)
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
            while(duplicate_check(user.customerID,customerdatabase,id_prefix) != 2)
            {
                temp_ID = generateID();
                
                //Converts variable "a" ^ to string and puts it in variable "customerID"
                sprintf(user.customerID, "%d", temp_ID);
            }
            
            //Inserting Data into their respective files
            int add_status = addcustomer(loginfile,customerdatabase,user.customerID,
                            user.firstname,user.lastname,user.email,
                            user.password,0);

            auditaddcustomer(add_status,user.customerID);
            

            system(clear_terminal); //Clears command line UI
            
            
            printf(underline_start"Would you like to LOGIN? \n"underline_end"'Y' for yes\n'N' for no\n\n");
            scanf(" %c", &registered);
        }
        else
        {
            printf("\n-Login database could not be accessed-");
        }
    
    return 0;
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
            system(clear_terminal); // Clears command line UI
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

        if(total_attempts == attempt )
        {
            close_console(delay_time,print_delay);
        }

        
        if(debug == 0)
        {
            printf("\nLogin Access Variable: %d\n",login_access);
        }

        //Check this variable when determining if the user should be given access to the program beyond this point
        if(login_access == 0)
        {
            if(debug == 0)
            {
                printf("login successful - customer ACCOUNT -\n");
                printf("==================================\n\n");
            }
            
            auditlogin(login_access,tempID_hold);
            customer_terminal(clear_terminal); //Starts up customer terminal

        }
        else if (login_access == 2)
        {
            if(debug == 0)
            {
                printf("\n\nlogin successful - admin ACCOUNT -\n\n"); 
                printf("==================================\n\n");
            }

            auditlogin(login_access,tempID_hold);
            admin_terminal(clear_terminal); //Starts up admin terminal
        }
        
    }
    //Terminate program
    else if(registered == 'N' || registered == 'n')
    {
        close_console(delay_time,print_delay);
    }

}

/************************* ADMIN TERMINAL  **********************/

void admin_terminal(char *terminal_clear_string)
{
    //Structure for all customer data
    typedef struct 
    {
        char customerID[max_length];
        char firstname[max_length];
        char lastname[max_length];
        char email[max_length];
        char password[password_length + 1];
        int income_class;
        
        //Basically, 1 customer can have up to 5 premises
        char premisesID[5][max_length];//Each record can be up to "max_length" long & can store 5 rows of data
        float meter_reading[5];
        int meter_size[5];

    } customer;

    customer user; // Declare struct variabile
    
    int action;
    int found_ID = 1; //Default to false
    int found_breakpoint = 1; //Default to false
    int premisesamt;

    char str[max_e_length];
    char strtemp[max_e_length];
    char strtemp2[max_e_length];
    char send_back_variable;//Will be used in if function to send user back to select admin actions
    char customers_to_view;
    char stall;
    char filename_temp[max_length];

    FILE *auditpointer; // Creates file pointer for audit file
    FILE *loginpointer; // Creates file pointer for login database file
    FILE *customerdbpointer; // Creates file pointer for customer database file
    FILE *temp_pointer; // Creates file pointer ( used in making edits to records )
    FILE *temp_pointer2; // Creates file pointer ( used in making edits to records )

    long int id_location;



    if(debug == 0)
    {
        printf("Program paused to give user chance to see debug\nEnter any character to continue:\n");
        scanf(" %c",&stall);
    }

jump_admin_actions: ;

    system(terminal_clear_string); //Clears command line UI

    printf(bold_start "\n---ADMIN TERMINAL---\n" bold_end);//Outputting header
    
    printf(underline_start"\nEnter choice of action:\n"underline_end);
    printf("(1) - Add customer/s\n");
    printf("(2) - Edit customer/s\n");
    printf("(3) - View customer/s\n");
    //printf("4 - Delete/Archive customer/s\n");
    //printf("5 - Generate Bill customer/s\n");
    //printf("6 - View reports\n");
    printf("(7) - View Audit Logs\n");
    printf("(8) - Close Terminal\n\n");
    scanf(" %d", &action);
    
    system(terminal_clear_string);  //Clears command line UI

    switch(action)
    {
        case 1: // Add customer/s
            printf("================== CUSTOMER DATABASE DEMO ==================\n"); //Outputting file header           
            printf(id_prefix"\n");
            printf(fname_prefix"\n");
            printf(lname_prefix"\n");
            printf("============================================================\n");

            printf(underline_start"\nEnter customer ID ( 7 digits ): \n"underline_end);
            scanf(" %s", user.customerID);

            //GETTINGS USER ID
            //Ensuring ID is atleast 7 digits long
            while(strlen(user.customerID)!=7)
            {
                printf(bold_start"\n-Min & Max ID length is (7) digits!-\n\n"bold_end);
                printf(underline_start"Enter new Customer ID:\n"underline_end);
                scanf(" %s", user.customerID);
            }

            //Ensuring no dupliciate IDs can be made
            while(duplicate_check(user.customerID,customerdatabase,id_prefix) != 2)
            {
                printf(bold_start"\n-ID already in use-\n"bold_end);
                printf(underline_start"\nEnter new Customer ID:\n"underline_end);
                scanf(" %s", user.customerID);
            }

            system(terminal_clear_string); // Clears command line UI

            printf("================== CUSTOMER DATABASE DEMO ==================\n"); //Outputting file header with user entered data     
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"\n");
            printf(lname_prefix"\n");
            printf("============================================================\n");

            printf(underline_start"\nEnter Customer First Name: \n"underline_end);
            scanf(" %s",user.firstname);

            system(terminal_clear_string); // Clears command line UI
            
            printf("================== CUSTOMER DATABASE DEMO ==================\n"); //Outputting file header with user entered data           
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"%s\n",user.firstname);
            printf(lname_prefix"\n");
            printf("============================================================\n");

            printf(underline_start"\nEnter Customer Last Name: \n"underline_end);
            scanf(" %s",user.lastname);

            system(terminal_clear_string); // Clears command line UI

            printf("================== CUSTOMER DATABASE DEMO ==================\n"underline_end); //Outputting file header with user entered data               
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"%s\n",user.firstname);
            printf(lname_prefix"%s\n",user.lastname);
            printf("============================================================\n\n");

            printf(underline_start"How many Premises would you like to add (Max 5): \n"underline_end);
            scanf(" %d", &premisesamt);

            //Ensuring user cannot select more than 5 premises to add
            while(premisesamt>5)
            {
                printf(bold_start"\n\nMaximum Number of premises per customer is 5\n\n"bold_end);
                printf(underline_start"How many Premises would you like to add (Max 5): \n"underline_end);
                scanf(" %d", &premisesamt);
            }

            //Gettings Premises details 
            for(int i = 0; i < premisesamt; i++)
            {
                printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                scanf(" %s",strtemp);

                //Ensuring Premises ID entered is 7 digits long
                while(strlen(strtemp)!=7)
                {
                    printf(bold_start"\n\n-Max & Min ID size is (7) digits-\n\n"bold_end);
                    printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                    scanf(" %s",strtemp);
                }


                //Ensuring no dupliciate Premises ID can be made
                while(duplicate_check(strtemp,customerdatabase,premisesid_prefix) != 2)
                {
                    printf(bold_start"\n-Premises not Available-\n\n"bold_end);
                    printf(underline_start"Enter new Premises ID #%d: \n"underline_end,i+1);
                    scanf(" %s",strtemp);
                }
                strcpy(user.premisesID[i],strtemp);


                printf(underline_start"\nEnter Meter size (150mm - 30mm - 15mm) #%d:\n"underline_end,i+1);
                scanf("%d",&user.meter_size[i]);

                //Ensuring user can only select one of 3 options available
                //Meter size 1 - 150
                //Meter size 2 - 30
                //Meter size 3 - 15
                while(user.meter_size[i] != meter1 && user.meter_size[i] != meter2 && user.meter_size[i] != meter3)
                {
                    printf(bold_start"\n-Invalid meter size-\n"bold_end);
                    printf(underline_start"\nEnter VALID Meter size (150mm - 30mm - 15mm) #%d:\n"underline_end, i+1);
                    scanf("%d",&user.meter_size[i]);
                }

                printf(underline_start"\nEnter Initial Meter Reading #%d:\n"underline_end,i+1);
                scanf("%f",&user.meter_reading[i]);

            }

            system(terminal_clear_string); // Clears command line UI

            printf("================== CUSTOMER DATABASE DEMO ==================\n"); //Outputting file header with user entered data              
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"%s\n",user.firstname);
            printf(lname_prefix"%s\n\n",user.lastname);

            for(int i = 0; i < premisesamt; i++)
            {
                printf("Premises ID #%d: %s\n",i+1, &user.premisesID[i][0]);
                printf("Meter Size #%d: %d\n",i+1,user.meter_size[i]);
                printf("Last Meter Reading #%d: %.2f\n",i+1, user.meter_reading[i]);
            }
            printf("============================================================\n");

            printf(bold_start"\nConfirm Addition of customer to database? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end);
            scanf(" %c", &send_back_variable);
            
            //ADDING CUSTOMER TO DATABASE
            if(send_back_variable == 'Y' || send_back_variable == 'y')
            {
                customerdbpointer = fopen(customerdatabase, "a");
                
                if(customerdbpointer!=NULL)
                {
                    //appends data passed to customer database
                    fputs(id_prefix, customerdbpointer);
                    fputs(user.customerID, customerdbpointer);
                    fputs("\n", customerdbpointer);
                        
                    fputs(fname_prefix, customerdbpointer);
                    fputs(user.firstname, customerdbpointer);
                    fputs("\n", customerdbpointer);
                    
                    fputs(lname_prefix, customerdbpointer);
                    fputs(user.lastname, customerdbpointer);
                    fputs("\n\n", customerdbpointer);
                    
                    for(int i = 0; i < premisesamt; i++)
                    {
                        clear_stringarray(strtemp);

                        snprintf(strtemp,max_e_length,premisesid_prefix"%d: %s",i+1, &user.premisesID[i][0]);
                        fputs(strtemp, customerdbpointer);
                        fputs("\n", customerdbpointer);
                        
                        clear_stringarray(strtemp);

                        snprintf(strtemp,max_e_length,metersize_prefix"%d: %d",i+1, user.meter_size[i]);  
                        fputs(strtemp, customerdbpointer);
                        fputs("\n", customerdbpointer);

                        clear_stringarray(strtemp);

                        snprintf(strtemp,max_e_length,lastmeter_r_prefix"%d: %.2f",i+1, user.meter_reading[i]);  
                        fputs(strtemp, customerdbpointer);
                        
                        //Prevents double line space at the end of user record
                        if(i != premisesamt-1) //Checks if i is approaching its max value
                        {
                            fputs("\n\n", customerdbpointer); 
                        }
                        else
                        {
                            fputs("\n", customerdbpointer); 
                        }

                    }
                    fputs(data_breakpoint, customerdbpointer);
                    fputs("\n\n", customerdbpointer);

                    printf(bold_start"\n-Data Added Successfully-\n"bold_end);

                    auditaddcustomer(1,user.customerID);

                    printf("\nWhen you're done enter X:\n");
                    scanf(" %c", &send_back_variable);

                    fclose(customerdbpointer);
                }
                else
                {
                    printf(file_open_error);
                }

                if(send_back_variable == 'X' || send_back_variable == 'x')
                {
                    goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                }
            }
            else if(send_back_variable == 'X' || send_back_variable == 'x')
            {
                goto jump_admin_actions; // Jumps code back to specified point if logic returns true
            }

            break;
            
        case 2: // Edit customer/s     

            //These long unsigned integers, will be used to store where their respective data in the txt file is found.
            long unsigned email_location = 0;
            long unsigned fname_location = 0;
            long unsigned lname_location = 0;
            long unsigned premisesID_location[5];
            int meter_size_location[5] = {0};
            int last_meter_r_location[5] = {0};
            int current_line;
            int user_has_email = 1; //1 for false - 0 for true
            premisesamt = 0; // Clear any value stored from previous actions

            printf(underline_start"Enter ID of customer you'd like to Edit:\n"underline_end);
            fflush(stdin);
            scanf(" %s", user.customerID);

            while(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2)
            //while duplicate data not found (information provided dosent exist in file checked)
            // do:
            {
                printf(bold_start"\n-No such ID exists-\n"bold_end);
                printf(underline_start"Enter ID of customer you'd like to Edit:\n"underline_end);
                fflush(stdin);
                scanf(" %s", user.customerID);
            }

            // attempts to open files in READ mode (r)
            customerdbpointer = fopen(customerdatabase, "r");
            loginpointer = fopen(loginfile,"r");

            found_breakpoint = 1; //Setting found breakpoint ";" back to false
            found_ID = 1; //Setting found ID back to false   
            
            if(customerdbpointer != NULL && loginpointer != NULL) // If both files were open successfully
            {               

                system(terminal_clear_string); // Clears command line UI

                //Printing & Saving User data selected
                printf("================== CUSTOMER DATABASE ==================\n\n"); //Outputting file header
                
                //Getting user Data
                current_line = 1;
                while(fgets(str,max_e_length,loginpointer)!=NULL && found_breakpoint != 0)
                {
                    if(strstr(str,user.customerID)!=NULL) //If id is found
                    {
                        found_ID = 0;
                    }
                    else if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
                    {
                        found_breakpoint = 0;
                    }
                    
                    //Getting email
                    if(found_ID == 0 && strstr(str,email_prefix) != NULL) 
                    //IF id has already been found and email prefix "User First Name: " has been found
                    //Do:
                    {
                        strcpy(user.email,str);
                        email_location = current_line;
                    }
                    current_line++;
                }

                found_breakpoint = 1; //Setting found breakpoint ";" back to false
                found_ID = 1; //Setting found ID back to false
                
                int i = 0;
                
                current_line = 1;
                while(fgets(str,max_e_length,customerdbpointer) != NULL && found_breakpoint != 0)
                {
                    if(strstr(str,user.customerID)!=NULL) //If id is found
                    {
                        found_ID = 0;
                    }

                    if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
                    {
                        found_breakpoint = 0;   
                        break;
                    }

                    //Getting first name
                    if(found_ID == 0 && strstr(str,fname_prefix) != NULL)
                    //IF id has already been found and fname prefix "User First Name: " has been found
                    //Do:
                    {
                        strcpy(user.firstname,str);
                        fname_location = current_line;
                    }
                    //Getting last name
                    else if(found_ID == 0 && strstr(str,lname_prefix) != NULL)
                    {
                        strcpy(user.lastname,str);
                        lname_location = current_line;
                    }

                    //Getting premises info
                    char final_prefix[max_length];
                    char i_as_string[1]; // i + terminating char "\0"
                    
                    if(found_ID == 0)
                    {
                        snprintf(i_as_string,12,"%d",i+1); // converts the current value of i to a string and stores it in "i_as_string"
                        // Check for Premises ID
                        if(strstr(str, premisesid_prefix) != NULL)
                        {
                            strcpy(user.premisesID[i], str); // Saves line with premises ID prefix
                            premisesID_location[i] = current_line; // Saves line number where premises id was found

                            //Assembles prefix for deletion
                            //String stored in "final_prefix" should look like "Premise ID #1: "
                            snprintf(final_prefix,max_length,premisesid_prefix"%d: ",i+1);

                            char *ptr = user.premisesID[i];

                            if(strstr(ptr,final_prefix)!=NULL)//If string to be deleted is found
                            {
                                for(int a = 0; a<(strlen(ptr)); a++)
                                {
                                    *(ptr+a) = *(ptr+(a+(strlen(final_prefix))));
                                }
                            }

                            strsanitize(ptr,0);
                            strcpy(user.premisesID[i],ptr);
                            ptr = NULL;
                            premisesamt++;

                        }
                        
                        // Check for Meter Size
                        else if (strstr(str, metersize_prefix) != NULL)
                        {
                            clear_stringarray(strtemp);
                            clear_stringarray(strtemp2);
                            clear_stringarray(final_prefix);

                            strcpy(strtemp2,metersize_prefix);
                            strcpy(strtemp,str);
                            meter_size_location[i] = current_line;

                            //Sanitizing Data retrieved
                            strncpy(final_prefix,strcat(strcat(strtemp2,i_as_string),": "),max_length);
                            remove_prefix(strtemp, final_prefix); // Remove prefix
                            user.meter_size[i] = atoi(strtemp); // Convert to integer
                        }
                        // Check for Last Meter Reading
                        else if (strstr(str, lastmeter_r_prefix) != NULL)
                        {
                            clear_stringarray(strtemp);
                            clear_stringarray(strtemp2);
                            clear_stringarray(final_prefix);

                            strcpy(strtemp2,lastmeter_r_prefix);
                            strcpy(strtemp, str);
                            last_meter_r_location[i] = current_line;

                            //Sanitizing Data retrieved
                            strncpy(final_prefix,strcat(strcat(strtemp2,i_as_string),": "),max_length);
                            remove_prefix(strtemp,final_prefix); // Remove prefix
                            user.meter_reading[i] = strtof(strtemp, NULL); // Convert to float
                            
                            i++; // Increment index after processing all fields for a premise
                        }
                        
                    }
                    current_line++;
                }
                
                //Removes prefix such as "UserID: " from string/s
                if(strlen(user.email)>0)
                {
                    remove_prefix(user.email,email_prefix);
                }
                remove_prefix(user.firstname,fname_prefix);
                remove_prefix(user.lastname,lname_prefix);

                //Removes "\n" from string/s
                strsanitize(user.email,0);
                strsanitize(user.firstname,0);
                strsanitize(user.lastname,0);

                ///Outputting current user data
                printf(text_red_start"UserID: %s\n"text_color_end,user.customerID);

                if(strlen(user.email)>11) // Email match was found
                {
                    printf("Email: %s\n",user.email);
                    user_has_email = 0; //set true
                }
                else // Email match was not found
                {
                    printf("Email: NO MATCH\n");
                    user_has_email = 1; //set false
                }
                printf("First name: %s\n",user.firstname);

                printf("Last name: %s\n",user.lastname);

                //Printing Premises data
                if(premisesamt>0)
                {
                    printf(bold_start"\nPREMISES OWNED %d\n"bold_end,premisesamt);
                    for(i = 0; i < premisesamt; i++)
                    {
                        printf(text_red_start premisesid_prefix"%d: %s\n"text_color_end,i+1, &user.premisesID[i][0]);
                        printf(metersize_prefix"%d: %d\n",i+1, user.meter_size[i]);
                        printf(lastmeter_r_prefix"%d: %.2f",i+1, user.meter_reading[i]); 
                            
                        //Prevents double line space at the end of user record
                        if(i != premisesamt-1)
                        {
                            printf("\n\n"); 
                        }
                        else
                        {
                            printf("\n"); 
                        }                      
                    }
                }

                printf("\n=======================================================\n");

                if(debug == 0)
                {
                    printf("\n=========== DEBUG DATA ===========\n");
                    printf("UserID (from user): %s\n",user.customerID);
                    printf("UserID strlen: %lu\n",strlen(user.customerID));
                    if(strlen(user.email)>11) // Email match was found
                    {
                        printf("\nEmail (from file): %s\n",user.email);
                        printf("Email location #: %lu\n",email_location);
                        printf("Email strlen: %lu\n",strlen(user.email));
                    }
                    else // Email match was not found
                    {
                        printf("\nEmail (from file): NO MATCH\n");
                        printf("Email location #: NO MATCH\n");
                        printf("Email strlen: NO MATCH\n");
                        
                    }
                    printf("\nFname (from file): %s\n",user.firstname);
                    printf("Fname location #: %lu\n",fname_location);
                    printf("Fname strlen: %lu\n",strlen(user.firstname));

                    printf("\nLname (from file): %s\n",user.lastname);
                    printf("Lname location #: %lu\n",lname_location);
                    printf("Lname strlen: %lu\n",strlen(user.lastname));

                    if(found_ID == 1)
                    {
                        printf("\nFound ID: FALSE\n");
                    }
                    else if( found_ID == 0)
                    {
                        printf("\nFound ID: TRUE\n");
                    }
                    if(found_breakpoint == 1)                                                                                                                                                                                        
                    {
                        printf("Found Breakpoint: FALSE\n");
                    }
                    else if( found_breakpoint == 0)
                    {
                        printf("Found Breakpoint: TRUE\n");
                    }

                    printf("Premises amount: %d\n",premisesamt);

                    

                    //Printing Premises data
                    if(premisesamt>0) // If premises were found
                    {
                        printf(bold_start"\nPremises info (from file):\n"bold_end);
                        for(i = 0; i < premisesamt; i++)
                        {
                            printf(premisesid_prefix"%d: %s\n",i+1, &user.premisesID[i][0]);
                            printf("Location line #: %lu\n",premisesID_location[i]);
                            printf(metersize_prefix"%d: %d\n",i+1, user.meter_size[i]);
                            printf("Location line #: %d\n",meter_size_location[i]);
                            printf(lastmeter_r_prefix"%d: %.2f\n",i+1, user.meter_reading[i]); 
                            printf("Location line #: %d",last_meter_r_location[i]);
                                
                            //Prevents double line space at the end of user record
                            if(i != premisesamt-1)
                            {
                                printf("\n\n"); 
                            }
                            else
                            {
                                printf("\n"); 
                            }                      
                        }
                    }
                    printf("==================================\n");
                }
                
                printf(underline_start"\nWhat would you like to edit? (text in "text_red_start"red, CANNOT"text_color_end underline_start" be edited):\n"underline_end);
                if(user_has_email != 1 && premisesamt > 0) // If user has email linked to ID/account and they own premises, do:
                {
                    printf("(1) - Email\n(2) - First Name\n(3) - Last name\n(4) - Meter Size\n(5) - Meter Reading\n(6) - All Fields\n");
                }
                else if(user_has_email == 1 && premisesamt > 0) //Has no email but has premises
                {
                    printf(text_red_start"(1) - Email"text_color_end"\n(2) - First Name\n(3) - Last name\n(4) - Meter Size\n(5) - Meter Reading\n(6) - All Fields\n");
                }
                else if (user_has_email != 1 && premisesamt == 0) //Has email but no premises
                {
                    printf("(1) - Email\n(2) - First Name\n(3) - Last name\n(4) - All Fields\n");
                }
                else if (user_has_email == 1 && premisesamt == 0) //Has no email and no premises
                {
                    printf(text_red_start"(1) - Email"text_color_end"\n(2) - First Name\n(3) - Last name\n(4) - All Fields\n");
                }
                
                
                int choice;
                int a;

                printf("(0) - Return To Actions Menu\n");

                //Getting users choice
                scanf(" %1d",&choice);

                clear_stringarray(strtemp);

                //Exit to action menu condition
                if(choice == 0)
                {
                    goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                }

                //CHANGING EMAIL
                else if(choice == 1)
                {
                    if(user_has_email != 1)
                    {
                        //Getting new email
                        printf(underline_start"\nEnter new email:\n"underline_end);
                        scanf(" %s",strtemp);

                        setall_lowercase(strtemp);
                            
                        //Validating email format by checking if the 3 most popular email domain names are included
                        while(strstr(strtemp,"@gmail.com") == NULL &&
                            strstr(strtemp,"@yahoo.com") == NULL &&
                            strstr(strtemp,"@hotmail.com") == NULL) //@<domain_name>.com was not found do:
                        {
                            printf("\n-Incorrect email format-\n\nAccepted domains:\n- @gmail.com\n- @yahoo.com\n- @hotmail.com\n\n");
                            printf(underline_start"Enter an email:\n"underline_end);
                            scanf("%s", strtemp);
                            setall_lowercase(strtemp);
                        }

                        /**Checks if the email entered is already being used and
                        asks the user to enter a new one if so**/
                        //On confirmation that email is available, user is prompted to enter password and their names.
                        if(duplicate_check(strtemp,loginfile,email_prefix) != 0)
                        {
                                
                            while(duplicate_check(strtemp,loginfile,email_prefix) != 2)
                            {
                                printf(bold_start"\n-Email already in use!-\n\n"bold_end);
                                printf(underline_start"Enter new email:\n"underline_end);
                                scanf("%s", strtemp);
                                setall_lowercase(strtemp);
                            }
                            
                        }
                        strcpy(user.email,strtemp);
                        clear_stringarray(strtemp);
                        clear_stringarray(filename_temp);

                        printf(bold_start"\nConfirm edit request for customer (%s) ? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end,user.customerID);
                        scanf(" %1c", &send_back_variable);

                        if(send_back_variable == 'Y' || send_back_variable == 'y')
                        {
                            //Creating temporary file to make edits
                            strcpy(filename_temp,loginfile); // Copies name of right variable into left
                        
                            int a = 0;
                            for(int b = 1; b<strlen(filename_temp); b++) // Removes .txt from end of file
                            {
                                if(a<2)
                                {
                                    filename_temp[strlen(filename_temp)-(b+a)] = '\0';
                                    a++;            
                                }
                        
                            }
                            strcat(filename_temp,"_temp"); // Adds "_temp" to the end of ^ ("customer_database" --> "customer_database_temp")
                            strcat(filename_temp,".txt"); // Adds back .txt to file name
                        
                            temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                            //Temporary file now created
                        
                            fseek(loginpointer,0,SEEK_SET); //Resests pointer to start of file

                            if(temp_pointer != NULL && loginpointer != NULL) // If both files open successfully, do:
                            {
                                current_line = 1;
                                while(fgets(str,max_e_length,loginpointer)!=NULL)
                                {
                                    if(current_line == email_location)
                                    {
                                        snprintf(strtemp,max_e_length,email_prefix"%s\n",user.email);
                                        fputs(strtemp,temp_pointer);
                                    }
                                    else
                                    {
                                        fputs(str,temp_pointer);
                                    }
                                    current_line++;
                                }
                            
                                fclose(loginpointer);
                                fclose(temp_pointer);
                                clear_stringarray(str);
                                clear_stringarray(strtemp);
                        
                                if(remove(loginfile) != 0)
                                {
                                    if(debug == 0)
                                    {
                                        perror("\nError msg");
                                        printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                    }
                                }  
                                else if(rename(filename_temp,loginfile) != 0)
                                {
                                    if(debug == 0)
                                    {
                                        perror("\nError msg");
                                        printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                    }
                                }
                                else
                                {
                                    printf(bold_start"\n-Record Modified Successfully-\n"bold_end);
                                    audit_editcustomer(choice,user.customerID);
                                }
                            }
                            else
                            {
                                printf(file_open_error);
                            }
                        }
                        else if (send_back_variable == 'X' || send_back_variable == 'x')
                        {
                            goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                        }


                    }
                    else
                    {
                        printf(text_red_start"\nNo email linked to user\n"text_color_end);
                    }
                }

                //CHANGING FIRST NAME
                else if (choice == 2)
                {
                    //Getting new name
                    printf(underline_start"\nEnter new first name:\n"underline_end);
                    scanf(" %s",user.firstname);

                    clear_stringarray(strtemp);
                    clear_stringarray(filename_temp);

                    printf(bold_start"\nConfirm edit request for customer (%s) ? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end,user.customerID);
                    scanf(" %1c", &send_back_variable);

                    if(send_back_variable == 'Y' || send_back_variable == 'y')
                    {
                        //Creating temporary file to make edits
                        strcpy(filename_temp,customerdatabase); // Copies name of right variable into left
                    
                        int a = 0;
                        for(int b = 1; b<strlen(filename_temp); b++) // Removes .txt from end of file
                        {
                            if(a<2)
                            {
                                filename_temp[strlen(filename_temp)-(b+a)] = '\0';
                                a++;            
                            }
                    
                        }
                        strcat(filename_temp,"_temp"); // Adds "_temp" to the end of ^ ("customer_database" --> "customer_database_temp")
                        strcat(filename_temp,".txt"); // Adds back .txt to file name
                    
                        temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                        //Temporary file now created
                    
                        fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                        
                        if(temp_pointer != NULL && customerdbpointer != NULL) // If both files open successfully
                        //do:
                        {
                            current_line = 1;
                            while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                            {
                                if(current_line == fname_location)
                                {
                                    snprintf(strtemp,max_e_length,fname_prefix"%s\n",user.firstname);
                                    fputs(strtemp,temp_pointer);
                                }
                                else
                                {
                                    fputs(str,temp_pointer);
                                }
                                current_line++;
                            }
                        
                            fclose(customerdbpointer);
                            fclose(temp_pointer);
                            clear_stringarray(str);
                            clear_stringarray(strtemp);
                    
                            if(remove(customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                }
                            }  
                            else if(rename(filename_temp,customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                }
                            }
                            else
                            {
                                printf(bold_start"\n-Record Modified Successfully-\n"bold_end);
                                audit_editcustomer(choice,user.customerID);
                            }
                        }
                        else
                        {
                            printf(file_open_error);
                        }
                    }
                    else if(send_back_variable == 'X' || send_back_variable == 'x')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }
                }

                //CHANGING LAST NAME
                else if (choice == 3)
                {
                    //Getting new name
                    printf(underline_start"\nEnter new last name:\n"underline_end);
                    scanf(" %s",user.lastname);

                    clear_stringarray(strtemp);
                    clear_stringarray(filename_temp);

                    printf(bold_start"\nConfirm edit request for customer (%s) ? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end,user.customerID);
                    scanf(" %1c", &send_back_variable);

                    if(send_back_variable == 'Y' || send_back_variable == 'y')
                    {
                        //Creating temporary file to make edits
                        strcpy(filename_temp,customerdatabase); // Copies name of right variable into left
                    
                        int a = 0;
                        for(int b = 1; b<strlen(filename_temp); b++) // Removes .txt from end of file
                        {
                            if(a<2)
                            {
                                filename_temp[strlen(filename_temp)-(b+a)] = '\0';
                                a++;            
                            }
                    
                        }
                        strcat(filename_temp,"_temp"); // Adds "_temp" to the end of ^ ("customer_database" --> "customer_database_temp")
                        strcat(filename_temp,".txt"); // Adds back .txt to file name
                    
                        temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                        //Temporary file now created
                    
                        fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                        
                        if(temp_pointer != NULL && customerdbpointer != NULL) // If both files open successfully
                        //do:
                        {
                            current_line = 1;
                            while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                            {
                                if(current_line == lname_location)
                                {
                                    snprintf(strtemp,max_e_length,lname_prefix"%s\n",user.lastname);
                                    fputs(strtemp,temp_pointer);
                                }
                                else
                                {
                                    fputs(str,temp_pointer);
                                }
                                current_line++;
                            }
                        
                            fclose(customerdbpointer);
                            fclose(temp_pointer);
                            clear_stringarray(str);
                            clear_stringarray(strtemp);
                    
                            if(remove(customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                }
                            }  
                            else if(rename(filename_temp,customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                }
                            }
                            else
                            {
                                printf(bold_start"\n-Record Modified Successfully-\n"bold_end);
                                audit_editcustomer(choice,user.customerID);
                            }
                        }
                        else
                        {
                            printf(file_open_error);
                        }
                    }
                    else if(send_back_variable == 'X' || send_back_variable == 'x')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }
                }

                //CHANGING METER SIZE
                else if (premisesamt > 0 && choice == 4)
                {
                    i = 0;
                    printf(underline_start"\nWhat meter # would you like to change?\n"underline_end);
                    scanf("%1d",&choice);

                    //Ensuring user can't edit meter that customer dosen't haves
                    while(choice > premisesamt)
                    {
                        printf(bold_start"\n-Customer owns no such meter #-\n"bold_end);
                        printf(underline_start"\nEnter VALID meter #:\n"underline_end);
                        scanf("%1d",&choice);
                    }

                    printf(underline_start"\nEnter New Meter size (150mm - 30mm - 15mm) #%d:\n"underline_end,choice);
                    scanf("%3d",&user.meter_size[i]);
    
                    //Ensuring user can only select one of 3 options available
                    //Meter size 1 - 150
                    //Meter size 2 - 30
                    //Meter size 3 - 15
                    while(user.meter_size[i] != meter1 && user.meter_size[i] != meter2 && user.meter_size[i] != meter3)
                    {
                        printf(bold_start"\n-Invalid meter size-\n"bold_end);
                        printf(underline_start"\nEnter VALID Meter size (150mm - 30mm - 15mm) #%d:\n"underline_end, i+1);
                        scanf("%3d",&user.meter_size[i]);
                    }

                    clear_stringarray(strtemp);
                    clear_stringarray(filename_temp);

                    printf(bold_start"\nConfirm edit request for customer (%s) ? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end,user.customerID);
                    scanf(" %1c", &send_back_variable);

                    if(send_back_variable == 'Y' || send_back_variable == 'y')
                    {
                        //Creating temporary file to make edits
                        strcpy(filename_temp,customerdatabase); // Copies name of right variable into left
                    
                        int a = 0;
                        for(int b = 1; b<strlen(filename_temp); b++) // Removes .txt from end of file
                        {
                            if(a<2)
                            {
                                filename_temp[strlen(filename_temp)-(b+a)] = '\0';
                                a++;            
                            }
                    
                        }
                        strcat(filename_temp,"_temp"); // Adds "_temp" to the end of ^ ("customer_database" --> "customer_database_temp")
                        strcat(filename_temp,".txt"); // Adds back .txt to file name
                    
                        temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                        //Temporary file now created
                    
                        fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                        
                        if(temp_pointer != NULL && customerdbpointer != NULL) // If both files open successfully
                        //do:
                        {
                            current_line = 1;
                            while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                            {
                                if(current_line == meter_size_location[choice-1])
                                {
                                    snprintf(strtemp,max_e_length,metersize_prefix"%d: %d\n",choice,user.meter_size[i]);
                                    fputs(strtemp,temp_pointer);
                                }
                                else
                                {
                                    fputs(str,temp_pointer);
                                }
                                current_line++;
                            }
                        
                            fclose(customerdbpointer);
                            fclose(temp_pointer);
                            clear_stringarray(str);
                            clear_stringarray(strtemp);
                    
                            if(remove(customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                }
                            }  
                            else if(rename(filename_temp,customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                }
                            }
                            else
                            {
                                printf(bold_start"\n-Record Modified Successfully-\n"bold_end);
                                audit_editcustomer(choice,user.customerID);
                            }
                        }
                    }
                    else if(send_back_variable == 'X' || send_back_variable == 'X')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }

                }

                //CHANGING METER READING
                else if (premisesamt > 0 && choice == 5)
                {
                    i = 0;
                    printf(underline_start"\nWhat meter reading # would you like to change?\n"underline_end);
                    scanf("%d",&choice);

                    //Ensuring user can't edit meter that customer dosen't haves
                    while(choice > premisesamt)
                    {
                        printf(bold_start"\n-Customer owns no such meter reading #-\n"bold_end);
                        printf(underline_start"\nEnter VALID meter reading #:\n"underline_end);
                        scanf("%d",&choice);
                    }

                    printf(underline_start"\nEnter New Meter reading:\n"underline_end);
                    scanf("%f",&user.meter_reading[i]);



                    clear_stringarray(strtemp);
                    clear_stringarray(filename_temp);

                    printf(bold_start"\nConfirm edit request for customer (%s) ? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end,user.customerID);
                    scanf(" %1c", &send_back_variable);

                    if(send_back_variable == 'Y' || send_back_variable == 'y')  
                    {
                        //Creating temporary file to make edits
                        strcpy(filename_temp,customerdatabase); // Copies name of right variable into left
                    
                        int a = 0;
                        for(int b = 1; b<strlen(filename_temp); b++) // Removes .txt from end of file
                        {
                            if(a<2)
                            {
                                filename_temp[strlen(filename_temp)-(b+a)] = '\0';
                                a++;            
                            }
                    
                        }
                        strcat(filename_temp,"_temp"); // Adds "_temp" to the end of ^ ("customer_database" --> "customer_database_temp")
                        strcat(filename_temp,".txt"); // Adds back .txt to file name
                    
                        temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                        //Temporary file now created
                    
                        fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                        
                        if(temp_pointer != NULL && customerdbpointer != NULL) // If both files open successfully
                        //do:
                        {
                            current_line = 1;
                            while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                            {
                                if(current_line == last_meter_r_location[choice-1])
                                {
                                    snprintf(strtemp,max_e_length,lastmeter_r_prefix"%d: %.2f\n",choice,user.meter_reading[i]);
                                    fputs(strtemp,temp_pointer);
                                }
                                else
                                {
                                    fputs(str,temp_pointer);
                                }
                                current_line++;
                            }
                        
                            fclose(customerdbpointer);
                            fclose(temp_pointer);
                            clear_stringarray(str);
                            clear_stringarray(strtemp);
                    
                            if(remove(customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                }
                            }  
                            else if(rename(filename_temp,customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                }
                            }
                            else
                            {
                                printf(bold_start"\n-Record Modified Successfully-\n"bold_end);
                                audit_editcustomer(choice,user.customerID);
                            }
                        }
                    }
                    else if(send_back_variable == 'X' || send_back_variable == 'x')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }

                }

                //CHANGING ALL FIELDS
                else if (premisesamt > 0 && choice == 6 || premisesamt == 0 && choice == 4)
                {
                    //Getting updated customer data
                    if(1)
                    {
                        //Getting new email
                        if(user_has_email != 1) // If user has email linked to ID/account, do:
                        {
                            printf(underline_start"\nEnter new email:\n"underline_end);
                            scanf(" %s",strtemp);

                            setall_lowercase(strtemp);
                                
                            //Validating email format by checking if the 3 most popular email domain names are included
                            while(strstr(strtemp,"@gmail.com") == NULL &&
                                strstr(strtemp,"@yahoo.com") == NULL &&
                                strstr(strtemp,"@hotmail.com") == NULL) //@<domain_name>.com was not found do:
                            {
                                printf("\n-Incorrect email format-\n\nAccepted domains:\n- @gmail.com\n- @yahoo.com\n- @hotmail.com\n\n");
                                printf(underline_start"Enter an email:\n"underline_end);
                                scanf("%s", strtemp);
                                setall_lowercase(strtemp);
                            }

                            /**Checks if the email entered is already being used and
                            asks the user to enter a new one if so**/
                            //On confirmation that email is available, user is prompted to enter password and their names.
                            if(duplicate_check(strtemp,loginfile,email_prefix) != 0)
                            {
                                    
                                while(duplicate_check(strtemp,loginfile,email_prefix) != 2)
                                {
                                    printf(bold_start"\n-Email already in use!-\n\n"bold_end);
                                    printf(underline_start"Enter new email:\n"underline_end);
                                    scanf("%s", strtemp);
                                    setall_lowercase(strtemp);
                                }
                                
                            }
                            strcpy(user.email,strtemp);
                            clear_stringarray(strtemp);
                        }
                    
                        //Getting new first name
                        printf(underline_start"\nEnter new first name:\n"underline_end);
                        scanf(" %s",user.firstname);

                        //Getting new last name
                        printf(underline_start"\nEnter new last name:\n"underline_end);
                        scanf(" %s",user.lastname);

                        //Getting new meter size & reading
                        for(int i = 0; i<premisesamt; i++)
                        {
                            printf(underline_start"\nEnter New Meter size (150mm - 30mm - 15mm) #%d:\n"underline_end,i+1);
                            scanf("%d",&user.meter_size[i]);
            
                            //Ensuring user can only select one of 3 options available
                            //Meter size 1 - 150
                            //Meter size 2 - 30
                            //Meter size 3 - 15
                            while(user.meter_size[i] != meter1 && user.meter_size[i] != meter2 && user.meter_size[i] != meter3)
                            {
                                printf(bold_start"\n-Invalid meter size-\n"bold_end);
                                printf(underline_start"\nEnter VALID Meter size (150mm - 30mm - 15mm) #%d:\n"underline_end, i+1);
                                scanf("%d",&user.meter_size[i]);
                            }

                            printf(underline_start"\nEnter New Last Meter Reading #%d\n"underline_end,i+1);
                            scanf(" %f",&user.meter_reading[i]);
                        }
                    }
                    
                    printf(bold_start"\nConfirm edit request for customer (%s) ? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end,user.customerID);
                    scanf(" %1c", &send_back_variable);

                    if(send_back_variable == 'Y' || send_back_variable == 'y')
                    {
                        //Creating strings for later use in "fputs()"
                        char tmpstrE[13][max_e_length]; // 13 records, all with a max character capacity of *max_e_length
                            
                        int next_pos;
                        if(user_has_email != 1)
                        {
                            snprintf(tmpstrE[0],max_e_length,email_prefix"%s\n",user.email); // Storing email string
                            snprintf(tmpstrE[1],max_e_length,fname_prefix"%s\n",user.firstname); // Storing first name string
                            snprintf(tmpstrE[2],max_e_length,lname_prefix"%s\n",user.lastname); // Storing last name
                            next_pos = 3;
                        }
                        else
                        {
                            snprintf(tmpstrE[0],max_e_length,fname_prefix"%s\n",user.firstname); // Storing first name string
                            snprintf(tmpstrE[1],max_e_length,lname_prefix"%s\n",user.lastname); // Storing last name
                            next_pos = 2;
                        }
                            
                        int it_amt = (premisesamt*2)+next_pos; // Calculating how many iterations are needed
                            
                        a = 0;
                        // Storing meter size string
                        for(int i = next_pos; i<it_amt; i+=2)
                        {
                            snprintf(tmpstrE[i],max_e_length,metersize_prefix"%d: %d\n", a+1 , user.meter_size[a]);
                            a++;
                        }
                            
                        // Storing meter reading string
                        a = 0;
                        for(int i = next_pos+1; i<it_amt; i+=2)
                        {
                            snprintf(tmpstrE[i],max_e_length,lastmeter_r_prefix"%d: %.2f\n", a+1 , user.meter_reading[a]); 
                            a++;
                        }
                        //Now we have all the updated formation to put in the string


                        //CREATING COPY OF CUSTOMBER & LOGIN DATABASE FILE TO USE IN EDITING
                        if(1) // If statement only here for organization ( I want to collapse certain parts of the code)
                        {
                            //CREATING COPY OF LOGIN FILE TO USE IN EDITING EMAIL
                            if(user_has_email != 1) // If user has email linked to ID/account, do:
                            {
                                
                                clear_stringarray(filename_temp);

                                ///Creating temporary file to make edits to login database
                                strcpy(filename_temp,loginfile); // Copies name of right variable into left
                            
                                int a = 0;
                                for(int b = 1; b<strlen(filename_temp); b++) // Removes .txt from end of file
                                {
                                    if(a<2)
                                    {
                                        filename_temp[strlen(filename_temp)-(b+a)] = '\0';
                                        a++;            
                                    }
                            
                                }
                                strcat(filename_temp,"_temp"); // Adds "_temp" to the end of ^ ("customer_database" --> "customer_database_temp")
                                strcat(filename_temp,".txt"); // Adds back .txt to file name
                            
                                temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                                //Temporary file for customer database now created

                                //Editing Email
                                clear_stringarray(str);
                                if(temp_pointer != NULL && loginpointer != NULL) // If both files open successfully
                                {
                                    if(user_has_email!=1)
                                    {
                                        fseek(loginpointer,0,SEEK_SET); //Resests pointer to start of file

                                        current_line = 1;
                                        while(fgets(str,max_e_length,loginpointer)!=NULL)
                                        {
                                            if(current_line == email_location)
                                            {
                                                fputs(tmpstrE[0],temp_pointer);
                                            }
                                            else
                                            {
                                                fputs(str,temp_pointer);
                                            }
                                            current_line++;
                                        }
                                    }
                                }
                                else
                                {
                                    printf(file_open_error);
                                }
                                
                                fclose(temp_pointer);
                                fclose(loginpointer);

                                // Deletes existing file & renames temporary file to original
                                if(remove(loginfile) != 0)
                                {
                                    if(debug == 0)
                                    {
                                        perror("\nError msg");
                                        printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                    }
                                }  
                                else if(rename(filename_temp,loginfile) != 0)
                                {
                                    if(debug == 0)
                                    {
                                        perror("\nError msg");
                                        printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                    }
                                }
                                else
                                {
                                    printf(bold_start"\n-Record Modified Successfully-\n"bold_end);
                                    audit_editcustomer(choice,user.customerID);
                                }
                                
                            }   

                            //CREATING COPY OF CUSTOMER DATABASE FILE TO USE IN EDITING EMAIL
                            clear_stringarray(filename_temp);
                            strcpy(filename_temp,customerdatabase); // Copies name of right variable into left
                        
                            int a = 0;
                            for(int b = 1; b<strlen(filename_temp); b++) // Removes .txt from end of file
                            {
                                if(a<2)
                                {
                                    filename_temp[strlen(filename_temp)-(b+a)] = '\0';
                                    a++;            
                                }
                        
                            }
                            strcat(filename_temp,"_temp"); // Adds "_temp" to the end of ^ ("customer_database" --> "customer_database_temp")
                            strcat(filename_temp,".txt"); // Adds back .txt to file name
                        
                            temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Writing & Reading mode (w+)
                            //Temporary file for customer database now created

                            if(debug == 0)
                            {        
                                printf("=========== DEBUG DATA ===========\n");      
                                printf("STRING PRE COMP TEST:\n");
                                printf("Premises Amt: %d\n\n",premisesamt);
                                
                                printf("tmpstrE string lookup:\n");
                                for( int i = 0; i<it_amt; i++)
                                {
                                    printf("Index (%d): %s",i,tmpstrE[i]);
                                }

                                if(user_has_email!=1)
                                {
                                    printf("\nEmail Line # to edit: %lu\n",email_location);
                                }
                                else
                                {
                                    printf("\n");
                                }
                                printf("First name Line # to edit: %lu\n",fname_location);
                                printf("Last name Line # to edit: %lu\n",lname_location);

                                for(i = 0; i < premisesamt; i++)
                                {
                                    printf("\nMeter size %d line #: %d\n",i+1,meter_size_location[i]);
                                    printf("Last Meter Reading %d line #: %d",i+1,last_meter_r_location[i]);
                                }
                                printf("\n==================================\n\n");
                            }
                            
                            //Editing all other fields
                            clear_stringarray(str);
                            int fault_check;
                            
                            fseek(customerdbpointer,0,SEEK_SET);
                            if(temp_pointer != NULL && customerdbpointer != NULL)
                            {
                                
                                fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                                current_line = 1;
                                i = 0;
                                a = 0;
                                while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                                //While there is more to read in file, do:
                                {
                                    if(current_line == fname_location)
                                    {
                                        fputs(tmpstrE[next_pos-2],temp_pointer);
                                    }
                                    else if(current_line == lname_location)
                                    {
                                        fputs(tmpstrE[next_pos-1],temp_pointer);
                                    }
                                    else if(current_line == meter_size_location[i])
                                    {
                                        fputs(tmpstrE[next_pos+a],temp_pointer);
                                    }
                                    else if(current_line == last_meter_r_location[i])
                                    {
                                        fputs(tmpstrE[(next_pos+1)+a],temp_pointer);
                                        i++; // Increments counter after both meter size and reading have been changed
                                        a+=2; // Increments counter after both meter size and reading have been changed
                                    }
                                    else
                                    {
                                        fputs(str,temp_pointer);
                                    }

                                    current_line++;
                                }

                            }
                            else
                            {
                                printf(file_open_error);
                            }

                            fclose(temp_pointer);
                            fclose(customerdbpointer);

                            clear_stringarray(str);
                            clear_stringarray(strtemp);
                    
                            if(remove(customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                }
                            }  
                            else if(rename(filename_temp,customerdatabase) != 0)
                            {
                                if(debug == 0)
                                {
                                    perror("\nError msg");
                                    printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                }
                            }
                            else
                            {
                                if(user_has_email != 0)
                                {
                                    printf(bold_start"\n-Record Modified Successfully-\n"bold_end);
                                    audit_editcustomer(choice,user.customerID);
                                }
                            }

                        }
                    }
                    else if (send_back_variable == 'X' || send_back_variable == 'x')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }
                }
            
            }
            
            printf("\nWhen you're done enter X:\n");
            scanf(" %c", &send_back_variable);
            if(send_back_variable == 'X' || send_back_variable == 'x')
            {

                goto jump_admin_actions; // Jumps code back to specified point if logic returns true
            }
            
            break;
            
        case 3: // View customer/s
            
            customerdbpointer = fopen(customerdatabase, "r"); // attempts to open file in READ mode (r)
            
            if(customerdbpointer!=NULL) // If File open attempt was successful
            {
                fseek(customerdbpointer,strlen("CUSTOMER DATABASE"),SEEK_SET); //Setting file pointer past header

                printf(underline_start"View all customers (A) or specific customer (P):\n"underline_end);
                scanf(" %c", &customers_to_view);
                
                while(customers_to_view != 'A' && customers_to_view != 'a' && customers_to_view != 'P' && customers_to_view != 'p')
                {
                    system(terminal_clear_string); // Clears command line UI
                    printf(bold_start"-Invalid Response-\n"bold_end);
                    printf(underline_start"View all customers (A) or specific customer (P):\n"underline_end);
                    scanf(" %c", &customers_to_view);
                }

                /// OUTPUTTING ALL CUSTOMER DATA 
                if(customers_to_view == 'A' || customers_to_view == 'a')
                // True if user enters nothing
                {
                    system(terminal_clear_string); //Clears command line UI
                    
                    printf("================== CUSTOMER DATABASE =================="); //Outputting file header


                    while(fgets(str, max_e_length,customerdbpointer) != NULL)
                    //Explanation: while fgets isnt at the end of the file, do:
                    {
                        if(strstr( str, data_breakpoint) == NULL)
                        /*Explanation: if "data_breakpoint" (which is ";") is not in current 
                        line then:
                        */
                        {
                            printf("%s",str);
                        }
                        else
                        {
                            printf("\n=======================================================\n");
                        }
                    }

                    printf("When you're done enter X:\n");
                    scanf(" %c", &send_back_variable);

                    if(send_back_variable == 'X' || send_back_variable == 'x')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }
                    
                }
                
                /// OUTPUTTING SPECIFIC CUSTOMER DATA
                else if(customers_to_view == 'P' || customers_to_view == 'p')
                {
                    printf(underline_start"Enter customerID to lookup:\n"underline_end);
                    scanf(" %s", user.customerID);

                    //Sanitizing values ( removing new line values, carriages, white space and other unwanted characters)
                    strsanitize(user.customerID,0);
                    
                    while(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2)
                    //while duplicate data not found (information provided dosent exist in file checked)
                    // do:
                    {
                        printf("\n-No such ID exists-\n\n");
                        printf(underline_start"Enter new ID to lookup:\n"underline_end);
                        scanf(" %s", user.customerID);
                        strsanitize(user.customerID,0);
                    }
                    
                    system(terminal_clear_string); // Clears command line UI
                    
                    printf("================== CUSTOMER DATABASE ==================\n\n"); //Outputting file header
                    
                    loginpointer = fopen(loginfile,"r");
                    
                    
                    //Resetting values
                    found_ID = 1;
                    found_breakpoint = 1;
                    //Resetting pointer
                    fseek(loginpointer,0,SEEK_SET);
                    
                    //Getting user Data
                    while(fgets(str,max_e_length,loginpointer)!=NULL && found_breakpoint != 0)
                    {
                        if(strstr(str,user.customerID)!=NULL) //If id is found
                        {
                            found_ID = 0;
                        }
                        else if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
                        {
                            found_breakpoint = 0;
                        }
                        
                        //Finding email
                        if(found_ID == 0 && strstr(str,email_prefix) != NULL) 
                        //IF id has already been found and email prefix "User First Name: " has been found
                        //Do:
                        {
                            strsanitize(str,0);
                            printf("%s\n",str);
                            break;
                        }
                    }
                    
                    //Resetting values
                    found_ID = 1;
                    found_breakpoint = 1;
                    //Resetting pointer
                    fseek(customerdbpointer,0,SEEK_SET);
                    
                    premisesamt = 0;
                    while(fgets(str, max_e_length,customerdbpointer) != NULL && found_breakpoint != 0) // Reads through entire file until the end is reached
                    {

                        if(strstr(str,user.customerID)!=NULL)
                        {
                            found_ID = 0;
                        }
                        
                        if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
                        {
                            found_breakpoint = 0;
                        }

                        if(strstr(str,premisesid_prefix) != NULL && found_ID == 0 && found_breakpoint != 0)
                        {
                            premisesamt++;
                        }
                        
                        if(found_ID == 0 && strstr(str,data_breakpoint) == NULL)
                        {
                            printf("%s",str);
                        }
                        
                    }
                    
                    printf("\n=======================================================\n");
                    printf(bold_start"PREMISES OWNED %d\n"bold_end,premisesamt);
                    printf("=======================================================\n\n");
                    
                }

                if(debug == 0)
                {           
                    printf("=========== DEBUG DATA ===========");
                    
                    printf("\ndata_breakpoint char: %s\n\n", data_breakpoint);
                    
                    //ID taken from user debug
                    if(customers_to_view == 'A' || customers_to_view == 'a')
                    {
                        printf("ID for lookup: NONE GIVEN\n");
                    }
                    else
                    {
                        printf("ID for lookup: %s\n",user.customerID);
                    }
                    printf("String length of ID: %lu\n\n",strlen(user.customerID));
                    
                    //ID info debug
                    if(found_ID == 0)
                    {
                        printf("Found ID: TRUE\n");
                    }
                    else
                    {
                        printf("Found ID: FALSE\n");
                    }
                    
                    //Breakpoint lookup debug
                    if(found_breakpoint == 0)
                    {
                        printf("Found Breakpoint: TRUE\n");
                    }
                    else
                    {
                        printf("Found Breakpoint: FALSE\n");
                    }       
                    printf("==================================\n\n");
                }

                fclose(loginpointer);
                fclose(customerdbpointer);

                printf("When you're done enter X:\n");
                scanf(" %c", &send_back_variable);
                
                if(send_back_variable == 'X' || send_back_variable == 'x')
                {
                    goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                }

            }
            else
            {
                printf(file_open_error);
            }
            
            break;
        case 4: // Delete/Archive customer/s
            //Code here
            break;
        case 5: // Generate Bill customer/s
            //Code here
            break;
        case 6: // View reports
            //Code here
            break;
            
        case 7: // View Audit Logs

            auditpointer = fopen(auditfile, "r"); // attempts to open file in READ mode (r)

            fseek(auditpointer,strlen("AUDIT LOGS"),SEEK_SET); //Setting file pointer past header to save a small amount of time
                    
            printf("================== AUDIT LOGS =================="); //Outputting file header

            while(fgets(str, max_e_length,auditpointer) != NULL)
            //Explanation: while fgets isnt at the end of the file, do:
            {
                if(strstr( str, data_breakpoint) == NULL)
                /*Explanation: if "data_breakpoint" (which is ";") is not in current 
                line then:
                */
                {
                    printf("%s",str);
                }
                else
                {
                    printf("\n================================================\n");
                }
            }
                
            if(debug == 0)
            {
                printf("=========== DEBUG DATA ===========\n");
                printf("data_breakpoint test: %s\n\n", data_breakpoint);
                printf("==================================\n\n");
            }
                
            printf("When you're done enter X:\n");
            scanf(" %c", &send_back_variable);

            fclose(auditpointer);

            if(send_back_variable == 'X' || send_back_variable == 'x')
            {
                goto jump_admin_actions; // Jumps code back to specified point if logic returns true
            }
            break;

        case 8: // Close terminal
        
            close_console(delay_time,print_delay);
            fclose(auditpointer);

            break;
    }

}

/************************* CUSTOMER TERMINAL  **********************/

void customer_terminal(char *terminal_clear_string)
{
    system(terminal_clear_string); //Clears command line UI

    printf(bold_start "\n---CUSTOMER TERMINAL---\n\n" bold_end);//Outputting header
    close_console(delay_time,print_delay);

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
int duplicate_check (char *data_to_check,char *filename,char *prefix_to_check)
{
    //Attempts to open file with name stored in "filename" in READ (r) mode
    FILE*fp = fopen(filename, "r");
    char str[max_e_length];
    int killstop = 1, func_validation = 0;

    if(fp != NULL)
    {
        while(fgets(str, max_e_length, fp)!=NULL)
        {
            
            if(strstr(str,data_to_check) != NULL && killstop != 0 && strstr(str,prefix_to_check) != NULL)
            {
                func_validation = 1; // Duplicate data was found
                killstop = 0;
            }
            else if (strstr(str,data_to_check) == NULL && killstop == 1)
            {
                func_validation = 2; // Duplicate data was not found
            }
        }
    }
    fclose(fp);
    return func_validation;
}

//Definition of function 3.
//  Return 0 if function executed successfully
//  Return 1 if function was unable to access a given file]
//  If end integer is set to 1, login database will be ignored and only customber database will be changed ( admin customer add)
int addcustomer (char *filename,char *filename2, char *customerID, char *user_firstname, char *user_lastname,char *user_email,char *user_password,int add_type)
{
    FILE *fp;
    char tmp_user_password[9];
            
    //attempts to open login Database file
    fp = fopen(filename, "a");
    if(fp != NULL)
    {
        if(add_type != 1 )//appends data passed through into file opened previously ^
        {
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
            fputs("\n", fp);
            fputs(data_breakpoint, fp);
            fputs("\n\n", fp);
        }
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
        fputs("\n", fp);
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
// Returns 2 if login sucessful for admin account
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
                strsanitize(tmpID_hold,1);

                if(debug == 0)
                {
                    printf("\n\n=========== DEBUG DATA ===========\n");
                    printf("Raw string from txt file (ID lookup): %s\n",tmpID_hold);
                    printf("userID pointer location: %ld\n", position1);
                }
            }
            
            if(strstr(str, "User Email: ") != NULL && strstr(str, email) != NULL)//If email is found
            {
                //Assigns email found at if statement to found_email
                char *found_email = strstr(str, email);
                
                if (found_email != NULL)
                {
                    strcpy(temp_email, found_email);
                    
                    if (debug == 0)
                    {
                        printf("\nRaw email string from txt file: %s",temp_email);
                        a = strlen(temp_email);
                        printf("strlen: %d", a);
                    }
                    
                    //Sanitizing values ( removing new line values, carriages and white space)
                    strsanitize(temp_email,1);
                    
                    if (debug == 0)
                    {
                        printf("\nEmail after sanitization (removing newline): %s\n",temp_email);
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
                            strsanitize(temp_password,1);

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
                            debug_a = 0; // Email matches
                            printf("\n\nEmail Type: ADMIN");
                        }
                        else
                        {
                            debug_a = 1; // Email dosen't match
                            printf("\n\nEmail Type: CUSTOMER");
                        }

                        
                        if(strcmp(temp_email,email)==0)
                        {
                            debug_a = 0;
                            printf("\nEmail Status: VALID MATCH");
                        }
                        else
                        {
                            debug_a = 1;
                            printf("\nEmail Status: NO MATCH");
                            
                        }
                        
                        if(strcmp(temp_password,hashed_password)==0)
                        {
                            debug_a = 0;
                            printf("\nPassword Status: VALID MATCH");
                        }
                        else
                        {
                            debug_a = 1;
                            printf("\nPassword Status: NO MATCH");
                        }

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
            if(fgets(str,max_e_length,fp) != NULL)//Goes down by online and stores string found in "str"
            {
                strcpy(tmpID_hold,str);
            }

            char *ptr = tmpID_hold;

            //Sanitizing string ( removing new line values(\n), carriages(\r), white space(" ") and other unwanted characters)
            if(strstr(ptr,id_prefix)!=NULL)//If string to be deleted is found
            {
                for(int i = 0; i<(strlen(ptr)); i++)
                {
                    //UserID: 5343075
                    *(ptr+i) = *(ptr+(i+(strlen(id_prefix))));
                }
            }

            strcpy(tempID,ptr);

            if (debug == 0)
            {
                printf("\nID send to audit function: %s\n",tempID);
            }
        }
        else if(fseek(fp,position1,SEEK_CUR) !=0 && debug == 0)
        {
            printf("\nfseek error\n");
        }
    }

    fclose(fp);
    return login_validation;
}

//Definition of function 6.
int generateID ()
{
    srand(time(NULL));
    // Generate random number between max ID value (9999999) and min ID value (1000000)
    long long ID = ((long long)rand()*rand())%((max_ID-min_ID)+1);

    //DEBUG
    if(debug == 0)
    {
        printf("\n\nFUNCTION ID TEST: %lld\n\n", ID);
    }
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
            fputs("Time: (24 hour)", fp);
            fputs(log.time, fp);
            fputs("\n", fp);

            fputs("New account ID: ", fp);
            fputs(customerID, fp);

            fputs("\n", fp);
            fputs(data_breakpoint, fp);
            fputs("\n\n", fp);
        }
        else if (audit_type == 1) //Customer was added successfully by admin
        {
            fputs("Account created (by admin)\n", fp);
                    
            //Format MONTH - DAY - YEAR
            fputs("Date: ", fp);
            fputs(log.date, fp);

            //Format HOUR - MINUTE
            fputs("Time: (24 hour)", fp);
            fputs(log.time, fp);
            fputs("\n", fp);

            fputs("New account ID: ", fp);
            fputs(customerID, fp);

            fputs("\n", fp);
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

//Defintion of function 12.
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
            fputs("Time (24 hour): ", fp);
            fputs(log.time, fp);
            fputs("\n", fp);

            fputs("ID of account login: ", fp);
            fputs(customerID, fp);

            fputs("\n", fp);
            fputs(data_breakpoint, fp);
            fputs("\n\n", fp);
        }
        else if (audit_type == 2) //admin user logged in successfully
        {
            fputs("Account login (by admin)\n", fp);
                    
            //Format MONTH - DAY - YEAR
            fputs("Date: ", fp);
            fputs(log.date, fp);

            //Format HOUR - MINUTE
            fputs("Time (24 hour): ", fp);
            fputs(log.time, fp);
            fputs("\n", fp);

            fputs("ID of account login: ", fp);
            fputs(customerID, fp);

            fputs("\n", fp);
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

//Defintion of function 13.
void delay_cpu(float delay){
    /* save start clock tick */
    const clock_t start = clock();

    clock_t current;
    do{
        /* get current clock tick */
        current = clock();

        /* break loop when the requested number of seconds have elapsed */
    }while((float)(current-start)/CLOCKS_PER_SEC < delay);
}

//Definition of function 14
void close_console(int delay_time_seconds, float print_t_delay)
{
    char hashtag = '#';

    //Emulates program closing down 
    printf("\nClosing Console: ");

    for(int i = 0; i < ((delay_time_seconds)*10); i ++)
    {
        printf("%c",hashtag);
        delay_cpu(print_t_delay);
    }

    exit(0);
}

void strsanitize(char *input_string,const int sanitize_type)
{
    if(sanitize_type == 0)
    {
        input_string[strcspn(input_string, "\n")] = '\0';
    }
    else if(sanitize_type == 1)
    {
        input_string[strcspn(input_string, "\r\n")] = '\0';
    }
}

void clear_stringarray(char *string_tobe_cleared)
{
    for(int i = 0; i<strlen(string_tobe_cleared); i++)
    {
        *(string_tobe_cleared+i) = 0;
    }
}

void remove_prefix(char *string_to_edit, char *prefix_to_remove)
{
    if(strstr(string_to_edit,prefix_to_remove)!=NULL)//If string to be deleted is found
    {
        for(int i = 0; i<(strlen(string_to_edit)); i++)
        {
            *(string_to_edit+i) = *(string_to_edit+(i+(strlen(prefix_to_remove))));
        }
    }
}

void audit_editcustomer(int audit_type, char * customerID)
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

        if (audit_type == 1) //Edit made to customer email
        {
            fputs("Edit made to customer email (by admin)\n", fp);
        }
        else if (audit_type == 2) //Edit made to customer first name
        {
            fputs("Edit made to customer first name (by admin)\n", fp);
        }
        else if (audit_type == 3) //Edit made to customer first name
        {
            fputs("Edit made to customer last name (by admin)\n", fp);
        }
        else if (audit_type == 4) //Edit made to customer first name
        {
            fputs("Edit made to customer meter size (by admin)\n", fp);
        }
        else if (audit_type == 5) //Edit made to customer first name
        {
            fputs("Edit made to customer meter reading (by admin)\n", fp);
        }
        else if(audit_type == 6)
        {
            fputs("Edit made to all customer fields (by admin)\n", fp);
        }

        //Format MONTH - DAY - YEAR
        fputs("Date: ", fp);
        fputs(log.date, fp);

        //Format HOUR - MINUTE
        fputs("Time (24 hour): ", fp);
        fputs(log.time, fp);
        fputs("\n", fp);

        fputs("ID of account edited: ", fp);
        fputs(customerID, fp);

        fputs("\n", fp);
        fputs(data_breakpoint, fp);
        fputs("\n\n", fp);

    }
    else
    {
        printf(file_open_error);
    }

    fclose(fp); 
}
