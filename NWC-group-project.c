/******************************************************************************
<Author Name>
Created Febraury 23, 2025

//If using linux, change "_getwch()" to "getch()"

//HARD CODED ADMIN 
Password "00000000"
Email "admin@gmail.com"

//NB FOR ACCOUNTS CREATED
New accounts added by admin terminal, automatically generates their gmail and password
gmail: <lastname><id#>@gmail.com 
password: <id#>@nzhi

user will be prompted to change this password upon login to their account.
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
#define file_open_error "\n-Error opening file-\n"
#define data_breakpoint ";"
#define max_ID 9999999
#define min_ID 1000000
#define max_card_amt 3

//Keyboard ASCII map
#define backspace_key 8
#define enter_key 13
#define tab_key 9
#define delete_key 127
#define space_key 32

//Database lookup prefix
#define id_prefix "UserID: "
#define fname_prefix "User First Name: "
#define lname_prefix "User Last Name: "
#define email_prefix "User Email: "
#define password_prefix "Hashed Password: "
#define premisesid_prefix "Premises ID #"
#define metersize_prefix "Meter Size #"
#define lastmeter_r_prefix "Last Meter Reading #"
#define status_prefix "User Status: "
#define income_class_prefix "Income Class: "
#define balance_tbp_prefix "Balance To Be Paid: "
#define balance_overdue_prefix "Balance Overdue: "
#define date_of_i_prefix "Date of Issue: "
#define due_d_prefix "Due Date: "
#define current_meter_r_prefix "Current Meter Reading: "
#define water_r_prefix "Water Rate: "
#define sewerage_r_prefix "Sewerage Rate: "
#define service_c_prefix "Service Charge: "
#define early_pr_prefix "Early Payment Reduction: " 
#define bill_status_prefix "Bill Status: " // 1 for unpaid & 0 for paid
#define surrender_prefix "-SR"
#define cycle_prefix "Cycle: "
#define reason_prefix "Reason: "
#define date_archived_prefix "Date Archived: "

//Initalizing variables
char registered;
char data_to_check[max_length];
char terminal;
char clear_terminal[5 +1];
char hashed_password[50];
int delay_time = 2;
float print_delay = .05;
char stall;

// set to 0 to see function outputs
// set to 1 to not see function outputs
int debug = 1;
int debug_scanfpassword = 1; // If linux is choosen at the start, this is set to 0

//Initializing file name/s
char *loginfile = "login_database.txt";
char *customerdatabase = "customer_database.txt";
char *auditfile = "audit_logs.txt";
char *billfile = "billing_logs.txt";
char *paymentcardfile = "payment_card_database.txt";

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
int user_login (char *email, char *password, char* tempID, int* is_user_new);

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

/*17. Function that takes in a string and a prefix to remove. if said prefix is found in string then
it is removed*/
void remove_prefix(char *string_to_edit, char *prefix_to_remove);

/*18. Function that takes in a customer ID as a string and checks if said customer is active or archived*/
// 0 if user is Active
// 1 if user is Archived
// 2 if user could not be found
int get_user_status(char* customerID_for_lookup);

/*19. Function that takes in a string and returns a 1 or 0 depending on if that string has a letter from the 
english alphabet in it*/
// Returns 1 if string has no letters
// Returns 0 if string has letters
int strhchar(char *string_to_check);

/**20. Function that sets all characters in an array to uppercase**/
void setall_uppercase (char* string);

/*21. Function that takes in a string and returns a 1 or 0 depending on if that string has a number in it*/
// Returns 0 if string has no numbers
// Returns 1 if string has numbers
int strhint(char *string_to_check);

/*22. Function that takes a premises ID as a string and checks if said id is surrendered*/
// 0 if premises is not available
// 1 if premises is available
// 2 if premises could not be found
int get_premises_status(char* premisesID_for_lookup);

//Terminals
void admin_terminal(char *terminal_clear_string);

void customer_terminal(char *terminal_clear_string, char* id_of_current_account, int is_user_new_account);


/*******************************************************************/

enum meter_size
{
    meter1 = 15, //15 millimeter
    meter2 = 30, //30 millimeter
    meter3 = 150, //150 millimeter
};

enum income_class
{
    /*Income class 1*/Low = 125, //(daily usage up to 125 L)
    /*Income class 2*/Low_Medium = 175, //(daily usage up to 175 L)
    /*Income class 3*/Medium = 220, //(daily usage up to 220 L)
    /*Income class 4*/Medium_High = 250, //(daily usage up to 250 L)
    /*Income class 5*/High = 300, //(daily usage up to 300 L)
};

struct audit
{
    char date[max_length];
    char time[max_length];   
};

//Structure for all customer data
typedef struct 
{
    char customerID[max_length];
    char email[max_length];
    char status[max_length];
    int incomeclass;
    char firstname[max_length];
    char lastname[max_length];
    int billing_cycle;
    float balance_overdue_amt;
    
    char password[password_length + 1];
    
    //Basically, 1 customer can have up to 5 premises
    char premisesID[5][max_length];//Each record can be up to "max_length" long & can store 5 rows of data
    float meter_reading[5];
    int meter_size[5];

    // Card info
    char card_number[max_card_amt][max_length]; // Customer can have up to 3 cards
    char cvv[max_card_amt][max_length];
    struct
    {
        int month[max_card_amt];
        int year[max_card_amt];
    }card_exp_date;

} customer;


//These long unsigned integers, will be used to store where their respective data in the txt file is found (as line numbers).
long unsigned email_location = 0;
long unsigned fname_location = 0;
long unsigned lname_location = 0;
long unsigned cycle_location = 0;
long unsigned balance_overdue_location = 0;
long unsigned user_status_location = 0;
long unsigned income_class_location = 0;
long unsigned premisesID_location[5];
long unsigned meter_size_location[5] = {0};
long unsigned last_meter_r_location[5] = {0};
long unsigned breakpoint_location = 0;

//related to storing bill location info
long unsigned userID_location = 0;
long unsigned date_oi_location = 0;
long unsigned due_date_location = 0;
long unsigned current_meter_r_location = 0;
long unsigned water_rate_location = 0;
long unsigned sewerage_rate_location = 0;
long unsigned service_c_location = 0;
long unsigned early_pr_location = 0;
long unsigned bill_status_location = 0;
long unsigned balance_tbp_location = 0;
// 

long unsigned current_line = 0;

/************************* MAIN TERMINAL  **********************/

void main()
{
  
    //Gets what terminal the user is running so the clear command line function "system()" works properly
    printf("-Are you on a windows or linux terminal -\n(L) for Linux\n(W) for Windows - Select this if unsure\n");
    scanf(" %c", &terminal);
    
    while(terminal != 'W' && terminal != 'w' && terminal != 'L' && terminal != 'l')
    {
        printf(bold_start"\nINVALID SELECTION\n"bold_end);
        printf("-Are you on a windows or linux terminal -\n(L) for Linux\n(W) for Windows - Select this if unsure\n");
        scanf(" %c", &terminal);
    }
    
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
                printf(file_open_error);
            }
               
            if(debug == 0)
            {
                printf("================ DEBUG DATA ================\n");
                printf("-Login database has been created-");
            }
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
                
            if(debug == 0)
            {
                printf(" starter data inserted-");
            }
        fclose(fp);
        
    }
    else
    {
        if(debug == 0)
        {
            printf("================ DEBUG DATA ================\n");
            printf("-Login Database found ( new file not created )-");
        }
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
            
            if(debug == 0)
            {
                printf("\n-Customer database has been created-");
            }
        fclose(fp);
            
        fp = fopen(customerdatabase, "a");
            const char *text = "CUSTOMER DATABASE\n\n";
            fputs(text, fp);
        
        fclose(fp);
        if(debug == 0)
        {
            printf(" starter data inserted-");
        }
    }
    else
    {
        if(debug == 0)
        {
            printf("\n-Customer Database found ( new file not created )-");
        }
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
                
            if(debug == 0)
            {
                printf("\n-Audit Logs have been created-");
            }
        fclose(fp);
            
        fp = fopen(auditfile, "a");
            const char *text = "AUDIT LOGS\n\n";
            fputs(text, fp);
        
        fclose(fp);
        if(debug == 0)
        {
            printf(" starter data inserted-");
        }
    }
    else
    {
        if(debug == 0)
        {
            printf("\n-Audit Logs found ( new file not created )-");
        }
    }
    
    //CREATING BILL LOGS
    //Checking if file exists, and creating customer database if not.
    if (does_file_exist(billfile) == 0)
    {
        // creating file using fopen() access mode "w"
        fp = fopen(billfile, "w");
            
            if(fp == NULL)
            {
                printf(file_open_error);
            }
                
            if(debug == 0)
            {
                printf("\n-Billing Logs have been created-");
            }
        fclose(fp);
            
        fp = fopen(billfile, "a");
            const char *text = "BILLING LOGS\n\n";
            fputs(text, fp);
        
        fclose(fp);
        if(debug == 0)
        {
            printf(" starter data inserted-\n");
        }
    }
    else
    {
        if(debug == 0)
        {
            printf("\n-Billing Logs found ( new file not created )-\n");
        }
    } 
    
    //CREATING PAYMENT CARD DATABASE
    //Checking if file exists, and creating customer database if not.
    if (does_file_exist(paymentcardfile) == 0)
    {
        // creating file using fopen() access mode "w"
        fp = fopen(paymentcardfile, "w");
        
        if(fp == NULL)
        {
            printf(file_open_error);
        }
            
        if(debug == 0)
        {
            printf("-Credit card database have been created-\n");
        }
        fclose(fp);
            
        fp = fopen(paymentcardfile, "a");
            const char *text = "USERID\t\tCARD#\t\tCVV#\t\tEXPIRY_M\t\tEXPIRY_Y\n";
            fputs(text, fp);
        
        fclose(fp);
        if(debug == 0)
        {
            printf(" starter data inserted-\n");
            printf("============================================\n");
        }
    }
    else
    {
        if(debug == 0)
        {
            printf("-Credit card database found ( new file not created )-\n");
            printf("============================================\n");
        }
    }  
/***************************** MAIN UI **************************/
    if(debug == 0)
    {
        printf("\n");
    }

    //Are you a registered user?
    printf("Are you a registered user? \n(Y) for yes\n(N) for no\n\n");
    scanf(" %c", &registered);
    
    while(registered!='N'&&registered!='n'&&registered!='Y'&&registered!='y')
    {
        fflush(stdin);
        system(clear_terminal);
        printf(bold_start"-Invalid input-\n"bold_end);
        printf("Are you a registered user? \n(Y) for yes\n(N) for no\n");
        scanf(" %c", &registered);
    }
    
    //structure variable <user>
    customer user;
    
    //CREATING NEW ACCOUNT
    if(registered=='N' || registered=='n')
    {
        
        
        system(clear_terminal); //Clears command line UI

        //Outputting header
        printf(bold_start"================== ACCOUNT REGISTRATION ==================\n\n"bold_end);
        
        
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
                printf(bold_start"\n-Email already in use!-\n\n"bold_end);
                printf(underline_start"Enter new email:\n"underline_end);
                scanf("%s", user.email);
                setall_lowercase(user.email);
            }
            

            strcpy(user.password,"temp");//Placeholder value so checks after this point works
            
            //Gets password from user
            while(strlen(user.password) < 8 || strlen(user.password)>31)
            {
                if(strlen(user.password)>31)
                {
                    printf(bold_start"\nPassword should be AT MOST 31 characters long\n"bold_end);
                    printf(underline_start"\nEnter new password \n( %d characters max ):\n"underline_end,password_length);
                    scanf(" %s",user.password);
                    continue;
                }
                printf(bold_start"\nPassword should be AT LEAST 8 characters long\n"bold_end);
                printf(underline_start"\nEnter new password \n( %d characters max ):\n"underline_end,password_length);
                scanf(" %s",user.password);
            }

            printf(underline_start"\nEnter your first name:\n"underline_end);
            scanf("%s", user.firstname);

            while(strhint(user.firstname) != 1)
            {
                printf(bold_start"\n-name cannot have numbers-\n"bold_end);
                printf(underline_start"\nEnter First Name: \n"underline_end);
                scanf(" %s",user.firstname);
            }            

            printf(underline_start"\nEnter your last name:\n"underline_end);
            scanf("%s", user.lastname);
            
            while(strhint(user.lastname) != 1)
            {
                printf(bold_start"\n-name cannot have numbers-\n"bold_end);
                printf(underline_start"\nEnter Last Name: \n"underline_end);
                scanf(" %s",user.lastname);
            }  

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
            
            printf(bold_start"================== ACCOUNT CREATED ==================\n\n"bold_end);
            printf(bold_start"Would you like to LOGIN? \n"bold_end"(Y) for yes\n(N) for no\n\n");
            scanf(" %c", &registered);
        }
        else
        {
            printf(bold_start"\n-Login database could not be accessed-"bold_end);
        }
    }

    
    //Existing account LOGIN
    int ch;
    int i = 0;
    int attempt = 2;
    int total_attempts = 6;
    int login_access = 1;
    char tempID_hold[max_length] = {0};
    int is_customer_new = 1; //Default to false
            
    if(registered == 'Y' || registered =='y')
    {
        
        system(clear_terminal); //Clears command line UI

        
        printf(bold_start"================== ACCOUNT LOGIN ==================\n\n"bold_end); //Outputting file header
        
        printf(underline_start"Enter your email:\n"underline_end);
        scanf(" %s", user.email);
        setall_lowercase(user.email);
        
        //Validating email format by checking if the 3 most popular email domain names are included
        while(strstr(user.email,"@gmail.com") == NULL &&
              strstr(user.email,"@yahoo.com") == NULL &&
              strstr(user.email,"@hotmail.com") == NULL && attempt < total_attempts-1) //@<domain_name>.com was not found do:
        {
            system(clear_terminal);
            printf(bold_start"================== ACCOUNT LOGIN ==================\n\n"bold_end); //Outputting file header
            
            printf("-Incorrect email format- attempts left: %d\n\nAccepted domains:\n- @gmail.com\n- @yahoo.com\n- @hotmail.com\n\n",(total_attempts-attempt)-1);
            printf(underline_start"Enter an email:\n"underline_end);
            scanf("%s", user.email);
            setall_lowercase(user.email);
            attempt++;
            
            if(attempt==total_attempts-1)
            {
                printf(bold_start"\nToo Many Invalid Attempts\n"bold_end);
                close_console(delay_time,print_delay);
            }
            
        }
        

        
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
        login_access = user_login(user.email,user.password,tempID_hold,&is_customer_new);
        
        // If user is archived then cancel successful login
        if(get_user_status(tempID_hold) == 1)
        {
            login_access = 1;
        }

        while(login_access != 0 && login_access != 2 && attempt < total_attempts)
        //While credentials given, dont link to a customer or user account
        {
            if(debug == 0)
            {
                printf("\nProgram paused to give user chance to see debug\nEnter any character to continue:\n");
                scanf(" %c",&stall);
            }
            system(clear_terminal); // Clears command line UI
            printf(bold_start"================== ACCOUNT LOGIN ==================\n"bold_end); //Outputting file header
            
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
            login_access = user_login(user.email,user.password,tempID_hold,&is_customer_new);

            if(get_user_status(tempID_hold) == 1)
            {
                login_access = 1;
            }
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
                printf(bold_start"===================================================\n\n"bold_end);
            }
            
            auditlogin(login_access,tempID_hold);
            customer_terminal(clear_terminal,tempID_hold,is_customer_new); //Starts up customer terminal

        }
        else if (login_access == 2)
        {
            if(debug == 0)
            {
                printf("login successful - admin ACCOUNT -\n"); 
                printf(bold_start"===================================================\n\n"bold_end);
            }

            auditlogin(login_access,tempID_hold);
            clear_stringarray(tempID_hold);
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
    customer user; // Declare struct variabile
    
    char action;
    int found_ID = 1; //Default to false
    int found_breakpoint = 1; //Default to false
    int premisesamt;

    char str[max_e_length];
    char strtemp[max_e_length];
    char strtemp2[max_e_length];
    char send_back_variable;//Will be used in if function to send user back to select admin actions
    char customers_to_view;
    char filename_temp[max_length];

    FILE *auditpointer; // Creates file pointer for audit file
    FILE *loginpointer; // Creates file pointer for login database file
    FILE *customerdbpointer; // Creates file pointer for customer database file
    FILE *billfilepointer; // Creates file pointer for billing logs database file
    FILE *temp_pointer; // Creates file pointer ( used in making edits to records )

    long int id_location;

    if(debug == 0)
    {
        printf("Program paused to give user chance to see debug\nEnter any character to continue:\n");
        scanf(" %c",&stall);
    }
    
jump_admin_actions:

    system(terminal_clear_string); //Clears command line UI
    //Outputting header
    printf(bold_start"================== ADMIN TERMINAL ==================\n"bold_end);
    
    int invalid_action = 0; // Default to true, assumes action is invalid
    int i = 0;
    action = ' ';

    while( action != '0' && action != '1' && action != '2' && action != '3' && action != '4' && action != '5' && action != '6' && action != '7')
    //While action is invalid
    {
        system(terminal_clear_string); // Clears command line UI
        printf(underline_start"\nEnter choice of action:\n"underline_end);
        printf("(1) - Add customer/s\n");
        printf("(2) - Edit customer/s\n");
        printf("(3) - View customer/s\n");
        printf("(4) - Delete/Archive customer/s\n");
        printf("(5) - Generate Bill customer/s\n");
        printf("(6) - View reports\n");
        printf("(7) - View Audit Logs\n");
        printf("(0) - Close Terminal\n\n");
        fflush(stdin);
        scanf(" %c", &action);
        
        if(action != '0' && action != '1' && action != '2' && action != '3' && action != '4' && action != '5' && action != '6' && action != '7')
        {
            printf(bold_start"-Input Choice not valid-\n"bold_end);
            printf(underline_start"\nEnter VALID choice of action:\n"underline_end);
            scanf(" %c", &action);
        }
    }
        
    system(terminal_clear_string); // Clears command line UI
    
    switch(action)
    {
        case '1': // Add customer/s
 
            printf(bold_start"================== CUSTOMER DATABASE DEMO ==================\n"bold_end); //Outputting file header           
            printf(id_prefix"\n");
            printf(fname_prefix"\n");
            printf(lname_prefix"\n");
            printf(bold_start"============================================================\n"bold_end);

            printf(underline_start"\nEnter customer ID ( 7 digits ): \n"underline_end);
            scanf(" %s", user.customerID);

            //GETTINGS USER ID
            //Ensuring ID is atleast 7 digits long
            while(strlen(user.customerID)!=7 || strhchar(user.customerID)==0)
            {
                if(strhchar(user.customerID)!=1)
                {
                    printf(bold_start"\n-ID cannot have letters-\n\n"bold_end);
                    printf(underline_start"Enter new Customer ID:\n"underline_end);
                    scanf(" %s", user.customerID);
                    continue;
                }
                printf(bold_start"\n-Min & Max ID length is (7) digits!-\n\n"bold_end);
                printf(underline_start"Enter new Customer ID:\n"underline_end);
                scanf(" %s", user.customerID);
            }

            //Ensuring no duplicate IDs can be made
            while(duplicate_check(user.customerID,customerdatabase,id_prefix) != 2)
            {
                printf(bold_start"\n-ID already in use-\n"bold_end);
                printf(underline_start"\nEnter new Customer ID:\n"underline_end);
                scanf(" %s", user.customerID);
            }

            system(terminal_clear_string); // Clears command line UI

            printf(bold_start"================== CUSTOMER DATABASE DEMO ==================\n"bold_end); //Outputting file header with user entered data     
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"\n");
            printf(lname_prefix"\n");
            printf(bold_start"============================================================\n"bold_end);

            printf(underline_start"\nEnter Customer First Name: \n"underline_end);
            scanf(" %s",user.firstname);

            while(strhint(user.firstname) != 1)
            {
                printf(bold_start"\n-Customer name cannot have numbers-\n"bold_end);
                printf(underline_start"\nEnter Customer First Name: \n"underline_end);
                scanf(" %s",user.firstname);
            }

            system(terminal_clear_string); // Clears command line UI
            
            printf(bold_start"================== CUSTOMER DATABASE DEMO ==================\n"bold_end); //Outputting file header with user entered data           
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"%s\n",user.firstname);
            printf(lname_prefix"\n");
            printf(bold_start"============================================================\n"bold_end);

            printf(underline_start"\nEnter Customer Last Name: \n"underline_end);
            scanf(" %s",user.lastname);

            while(strhint(user.lastname) != 1)
            {
                printf(bold_start"\n-Customer name cannot have numbers-\n"bold_end);
                printf(underline_start"\nEnter Customer Last Name: \n"underline_end);
                scanf(" %s",user.lastname);
            }

            system(terminal_clear_string); // Clears command line UI

            printf(bold_start"================== CUSTOMER DATABASE DEMO ==================\n"bold_end); //Outputting file header with user entered data               
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"%s\n",user.firstname);
            printf(lname_prefix"%s\n",user.lastname);
            printf(bold_start"============================================================\n\n"bold_end);

            printf(underline_start"How many Premises would you like to add (Max 5): \n"underline_end);
            scanf(" %d", &premisesamt);

            //Ensuring user cannot select more than 5 premises to add
            while(premisesamt>5)
            {
                printf(bold_start"\nMaximum Number of premises per customer is 5\n\n"bold_end);
                printf(underline_start"How many Premises would you like to add (Max 5): \n"underline_end);
                scanf(" %d", &premisesamt);
            }

            //Gettings Premises details 
            char temp_premisesID_hold[5][max_e_length] = {0}; // Variable used to store premises numbers already entered
            int id_record_exists = 1; // Default to false 
            int redo_gettingID; 
            for(i = 0; i < premisesamt; i++)    // and check to ensure current premises IDs  being entered, dont match past ones
            {
                system(terminal_clear_string); // Clears command line UI

                printf(bold_start"================== CUSTOMER DATABASE DEMO ==================\n"bold_end); //Outputting file header with user entered data               
                printf(id_prefix"%s\n",user.customerID);
                printf(fname_prefix"%s\n",user.firstname);
                printf(lname_prefix"%s\n",user.lastname);
                printf(bold_start"============================================================\n"bold_end);

                if(redo_gettingID == 0)
                {
                    printf(bold_start"\n-Premises ID already entered-\n"bold_end);
                    printf(underline_start"\nEnter NEW premises ID (7 digits) #%d: \n"underline_end,i+1);
                    scanf(" %s",strtemp);
                }
                else
                {
                    printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                    scanf(" %s",strtemp);
                }
                
                redo_gettingID = 1; // Default to false

                //Ensuring Premises ID entered is 7 digits long and contains only numbers
                while(strlen(strtemp)!=7 || strhchar(strtemp) != 1)
                {
                    if(strhchar(strtemp) != 1)
                    {
                        printf(bold_start"\n-Premises ID cannot contain letters\n"bold_end);
                        printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                        scanf(" %s",strtemp);
                        continue;
                    }
                    printf(bold_start"\n-Max & Min ID size is (7) digits-\n"bold_end);
                    printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                    scanf(" %s",strtemp);
                }
                
                //Ensuring no dupliciate Premises ID can be made
                while(duplicate_check(strtemp,customerdatabase,premisesid_prefix) != 2)
                {
                    // Checking if duplicate ID is surrendered
                    if(get_premises_status(strtemp)!=1)
                    // ID found was not surrendered, do:
                    {
                        printf(bold_start"\n-Premises In Use-\n\n"bold_end);
                        printf(underline_start"Enter New Premises ID #%d: \n"underline_end,i+1);
                        scanf(" %s",strtemp);
                    }
                    else
                    {
                        id_record_exists = 0; // Premises ID is available
                        break; // Premises ID is available as the other instance found is marked as surrendered
                    }
                }
                
                strcpy(temp_premisesID_hold[i],strtemp);
                
                if(debug == 0)
                {
                    printf("\nPrevious user entered IDS: ");
                    for(int c = 0; c < i; c++)
                    {
                        printf("%s,",temp_premisesID_hold[c]);
                    } 
                    printf("\nCurrent ID: %s\n",strtemp);
                    printf("\nCurrent index (i): %d\n",i);
                }
                
                // Checking if the premises entered was already entered previously
                if(i > 0) //If we're not entering our first premises, do:
                {
                    for(int a = 0; a<i; a++)
                    {
                        if(strcmp(temp_premisesID_hold[a],strtemp) == 0) // If a matching premises is found
                        {
                            redo_gettingID = 0; //Set to true
                            break;
                        }
                    }
                }
                if(redo_gettingID == 0)
                {
                    i--; //decrement i such that we can re-enter premises id which wouldve been stored at current index 
                    continue;
                }
                else
                {
                    strcpy(user.premisesID[i],strtemp);
                    
                    char choice;
                    int temp_mtr_size=123; // value set to check if code works (gets correct data from file)
                    float temp_lmtr_reading=1234;// value set to check if code works (gets correct data from file)
                    int temp_premises_amt = 0;

                    //Using previous meter size from file
                    if(id_record_exists != 1)
                    {
                        customerdbpointer = fopen(customerdatabase,"r"); // attempts to open file in READ mode (r)

                        found_ID = 1; //Default to false
                        found_breakpoint = 1; //Default to false
                        
                        clear_stringarray(strtemp);
                        if(customerdbpointer != NULL)
                        {

                            while(fgets(str,max_e_length,customerdbpointer)!= NULL && found_breakpoint != 0)
                            {
                                if(strstr(str,premisesid_prefix) != NULL && strstr(str,user.premisesID[i]) != NULL) // If id is found
                                {
                                    found_ID = 0;
                                }
                                if(strstr(str,data_breakpoint) != NULL && found_ID == 0)
                                {
                                    found_breakpoint = 0;
                                    break;
                                }

                                //Getting specific premises info
                                if(found_ID == 0)
                                {
                                    if(strstr(str,metersize_prefix))
                                    {
                                        // Getting current meter size index number
                                        strncpy(strtemp2,str,max_e_length);
                                        remove_prefix(strtemp2,metersize_prefix);
                                        strtemp2[1] = '\0'; // Ensures only the first character is kept
                                        temp_premises_amt = atoi(strtemp2);

                                        // Getting the meter size
                                        snprintf(strtemp2,max_e_length,metersize_prefix"%d: ",temp_premises_amt);
                                        remove_prefix(str,strtemp2);
                                        temp_mtr_size = atoi(str);
                                    }
                                    else if(strstr(str,lastmeter_r_prefix))
                                    {
                                        // Getting current meter reading
                                        snprintf(strtemp2,max_e_length,lastmeter_r_prefix"%d: ",temp_premises_amt);
                                        remove_prefix(str,strtemp2);
                                        temp_lmtr_reading = atof(str);
                                    }
                                }
                            }
                            fclose(customerdbpointer);

                            printf("\nPrevious meter size found for premises ID (%s)\nWould you like to use it? (Y) Yes OR (N) No\n",user.premisesID[i]);
                            scanf(" %c", &choice);

                            while(choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n')
                            {
                                printf(bold_start"\n-Invalid Selection-\n"bold_end);
                                printf("Would you like to use it? (Y) Yes OR (N) No\n");
                                scanf(" %c", &choice);
                            }

                            if(choice == 'Y' || choice == 'y')
                            {
                                user.meter_size[i] = temp_mtr_size;  
                            }
                        }
                        else
                        {
                            printf(file_open_error);
                        }
                    }

                    // Getting new size
                    if(id_record_exists == 1 || choice == 'N' || choice == 'n')
                    {
                        printf(underline_start"\nEnter Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,i+1);
                        scanf("%d",&user.meter_size[i]);

                        //Ensuring user can only select one of 3 options available
                        //Meter size 1 - 150
                        //Meter size 2 - 30
                        //Meter size 3 - 15
                        while(user.meter_size[i] != meter1 && user.meter_size[i] != meter2 && user.meter_size[i] != meter3)
                        {
                            printf(bold_start"\n-Invalid meter size-\n"bold_end);
                            printf(underline_start"\nEnter VALID Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,i+1);
                            scanf("%d",&user.meter_size[i]);
                        }
                    }
                    
                    //Using previous meter reading from file
                    if(id_record_exists != 1)
                    {
                        printf("\nPrevious meter reading found for premises ID (%s)\nWould you like to use it? (Y) Yes OR (N) No\n",user.premisesID[i]);
                        scanf(" %c", &choice);

                        while(choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n')
                        {
                            printf(bold_start"\n-Invalid Selection-\n"bold_end);
                            printf("Would you like to use it? (Y) Yes OR (N) No\n");
                            scanf(" %c", &choice);
                        }

                        if(choice == 'Y' || choice == 'y')
                        {
                            user.meter_reading[i] = temp_lmtr_reading;  
                        }
                    }

                    // Getting new reading
                    char reading_temp_hold[max_length] = {0};
                    if(id_record_exists == 1 || choice == 'N' || choice == 'n')
                    {
                        printf(underline_start"\nEnter Initial Meter Reading #%d:\n"underline_end,i+1);
                        scanf(" %s",reading_temp_hold);

                        while(strhchar(reading_temp_hold) != 1 || atof(reading_temp_hold)<0)
                        {
                            if(atof(reading_temp_hold)<0)
                            {
                                printf(bold_start"\n-Reading cannot be negative-\n"bold_end);
                                printf(underline_start"\nEnter VALID Initial Meter Reading #%d:\n"underline_end,i+1);
                                scanf(" %s",reading_temp_hold);
                                continue;
                            }
                            
                            printf(bold_start"\n-Reading can only be a number-\n"bold_end);
                            printf(underline_start"\nEnter VALID Initial Meter Reading #%d:\n"underline_end,i+1);
                            scanf(" %s",reading_temp_hold);
                            
                        }
                        user.meter_reading[i] = atof(reading_temp_hold);
                    }
                    
                    
                }
                
            }

            system(terminal_clear_string); // Clears command line UI

            printf(bold_start"================== CUSTOMER DATABASE DEMO ==================\n"bold_end); //Outputting file header with user entered data              
            printf(id_prefix"%s\n",user.customerID);
            printf(fname_prefix"%s\n",user.firstname);
            printf(lname_prefix"%s\n",user.lastname);
            
            if(premisesamt>0)
            {
                printf("\n");
            }

            for(int i = 0; i < premisesamt; i++)
            {
                printf("Premises ID #%d: %s\n",i+1, &user.premisesID[i][0]);
                printf("Meter Size #%d: %d\n",i+1,user.meter_size[i]);
                printf("Last Meter Reading #%d: %.2f",i+1, user.meter_reading[i]);
                if(i == premisesamt-1)
                {
                    printf("\n");
                }
                else
                {
                    printf("\n\n");
                }
            }
            printf(bold_start"============================================================\n"bold_end);

            printf(bold_start"\nConfirm Addition of customer to database? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"bold_end);
            scanf(" %c", &send_back_variable);
            
            //ADDING CUSTOMER TO DATABASE
            if(send_back_variable == 'Y' || send_back_variable == 'y')
            {
                customerdbpointer = fopen(customerdatabase, "a");
                loginpointer = fopen(loginfile, "a");
                
                //Adding customer to customer database
                if(customerdbpointer != NULL)
                {
                    
                    //appends data passed to customer database
                    fputs(id_prefix, customerdbpointer);
                    fputs(user.customerID, customerdbpointer);
                    fputs("\n", customerdbpointer);

                    fputs(status_prefix, customerdbpointer);
                    fputs("ACTIVE", customerdbpointer);
                    fputs("\n", customerdbpointer);

                    // generates maximum number, between max and min
                    int random_number;
                    int max = 5;
                    int min = 1;
                    srand(time(NULL));
                    random_number = min + rand() % ((max - min) + 1); 
                    clear_stringarray(strtemp);
                    snprintf(strtemp,max_e_length,"%d",random_number); // converts right-most variable into string thats placed in left-most

                    fputs(income_class_prefix, customerdbpointer);
                    fputs(strtemp, customerdbpointer); // Puts random number as income class
                    fputs("\n", customerdbpointer);
                        
                    fputs(fname_prefix, customerdbpointer);
                    fputs(user.firstname, customerdbpointer);
                    fputs("\n", customerdbpointer);
                    
                    fputs(lname_prefix, customerdbpointer);
                    fputs(user.lastname, customerdbpointer);
                    fputs("\n", customerdbpointer);

                    fputs(cycle_prefix, customerdbpointer);
                    fputs("0", customerdbpointer);
                    fputs("\n", customerdbpointer);

                    fputs(balance_overdue_prefix, customerdbpointer);
                    fputs("0", customerdbpointer);
                    fputs("\n", customerdbpointer);
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
                        fputs("\n", customerdbpointer);
                        
                    }
                    
                    fputs(data_breakpoint, customerdbpointer);
                    fputs("\n", customerdbpointer);
                    fclose(customerdbpointer);
                }
                else
                {
                    printf(file_open_error);
                }

                //Adding customer to login database
                if(loginpointer != NULL )
                {
                    //appends data passed to login database
                    fputs(id_prefix, loginpointer);
                    fputs(user.customerID, loginpointer);
                    fputs("\n", loginpointer);

                    //Creates email as a combination of last name and customer ID
                    snprintf(strtemp,max_e_length,"%s%s@gmail.com",user.lastname,user.customerID);
                    strcpy(user.email,strtemp);
                    setall_lowercase(user.email);
                    fputs(email_prefix, loginpointer);
                    fputs(user.email, loginpointer);
                    fputs("\n", loginpointer);
                    
                    snprintf(strtemp,max_e_length,"%s@nzhi",user.customerID);
                    hash_djb2(strtemp,user.password);
                    fputs(password_prefix, loginpointer);
                    fputs(user.password, loginpointer);
                    fputs("\n", loginpointer);
                    
                    fputs(data_breakpoint, loginpointer);
                    fputs("\n", loginpointer);
                    
                    printf(bold_start"\n-Customer Added Successfully-\n\n"bold_end);
                    auditaddcustomer(1,user.customerID);
                    fclose(loginpointer);
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

            break;
            
        case '2': // Edit customer/s     
            int user_has_email = 1; //1 for false - 0 for true
            int fault_check;
            int surrendered_premises_amt = 0;
            premisesamt = 0; // Clear any value stored from previous actions
            
            printf(bold_start"================== CUSTOMER DATABASE ==================\n\n"bold_end); //Outputting file header
            printf(underline_start"Enter ID of customer you'd like to Edit:\n"underline_end);
            fflush(stdin);
            scanf(" %s", user.customerID);
            
            while(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2 || get_user_status(user.customerID) != 0)
            //while duplicate data not found (information provided dosent exist in file checked)
            // do:
            {
                if(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2)
                {
                    printf(bold_start"\n-No Such Customer Exists-\n\n"bold_end);
                }
                else if(get_user_status(user.customerID) != 0)
                {
                    printf(bold_start"\n-This customer account is archived-\n\n"bold_end);
                    printf(underline_start"Enter ID of ACTIVE customer you'd like to Edit:\n"underline_end);
                    fflush(stdin);
                    scanf(" %s", user.customerID);
                    continue;
                }
                
                printf(underline_start"Enter VALID ID of customer you'd like to Edit:\n"underline_end);
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

                printf(bold_start"================== CUSTOMER DATABASE ==================\n\n"bold_end); //Outputting file header
                
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
                        breakpoint_location = current_line;
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
                    else if(found_ID == 0 && strstr(str,balance_overdue_prefix) != NULL)
                    {
                        clear_stringarray(strtemp);
                        strcpy(strtemp,str);
                        remove_prefix(strtemp,balance_overdue_prefix);
                        user.balance_overdue_amt = atof(strtemp); // converts income class from str to float
                        balance_overdue_location = current_line;
                    }
                    //Getting account status
                    else if(found_ID == 0 && strstr(str,status_prefix) != NULL)
                    {
                        strcpy(user.status,str);
                        user_status_location = current_line;
                    }
                    //Getting income class
                    else if(found_ID == 0 && strstr(str,income_class_prefix) != NULL)
                    {
                        clear_stringarray(strtemp);
                        strcpy(strtemp,str);
                        remove_prefix(strtemp,income_class_prefix);
                        user.incomeclass = atoi(strtemp); // converts income class from str to int
                        income_class_location = current_line;
                    }

                    //Getting premises info
                    char final_prefix[max_length];
                    char i_as_string[1]; // i + terminating char "\0"
                    surrendered_premises_amt = 0;
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
                    
                            if(strstr(str,surrender_prefix)!=NULL)
                            {
                                surrendered_premises_amt++;
                            }
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
                remove_prefix(user.status,status_prefix);

                //Removes "\n" from string/s
                strsanitize(user.email,0);
                strsanitize(user.firstname,0);
                strsanitize(user.lastname,0);
                strsanitize(user.status,0);

                ///Outputting current user data
                printf(text_red_start id_prefix"%s\n"text_color_end,user.customerID);
                if(strlen(user.email)>11) // Email match was found
                {
                    printf(email_prefix"%s\n",user.email);
                    user_has_email = 0; //set true
                }
                else // Email match was not found
                {
                    printf(email_prefix"NO MATCH\n");
                    user_has_email = 1; //set false
                }
                printf(income_class_prefix"%d\n",user.incomeclass);
                printf(fname_prefix"%s\n",user.firstname);

                printf(lname_prefix"%s\n",user.lastname);
                printf(text_red_start balance_overdue_prefix"$%.2f\n"text_color_end,user.balance_overdue_amt);

                //Printing Premises data
                if(premisesamt>0)
                {
                    if(premisesamt-surrendered_premises_amt>0)
                    {
                        printf(bold_start"\nPREMISES OWNED %d\n"bold_end,premisesamt-surrendered_premises_amt);
                    }
                    for(i = 0; i < premisesamt; i++)
                    {
                        if(strstr(user.premisesID[i],surrender_prefix)==NULL)
                        {
                            printf(text_red_start premisesid_prefix"%d: %s\n"text_color_end,i+1, &user.premisesID[i]);
                            printf(metersize_prefix"%d: %d\n",i+1, user.meter_size[i]);
                            printf(lastmeter_r_prefix"%d: %.2f",i+1, user.meter_reading[i]); 
                        }
                        else
                        {
                            continue; //If premises ID is marked for surrender, skip to next iteration
                        } 

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

                printf(bold_start"\n=======================================================\n"bold_end);
                printf(underline_start"INCOME CLASS\n"underline_end);
                printf("1 - Low: daily usage of up to %d litres\n",Low);
                printf("2 - Low Medium: daily usage of up to %d litres\n",Low_Medium);
                printf("3 - Medium: daily usage of up to %d litres\n",Medium);
                printf("4 - Medium High: daily usage of up to %d litres\n",Medium_High);
                printf("5 - High: daily usage of up to %d litres\n",High);
                printf("=======================================================\n");

                if(debug == 0)
                {
                    printf("\n=========== DEBUG DATA ===========\n");
                    printf("UserID (from user): %s\n",user.customerID);
                    printf("UserID strlen: %lu\n",strlen(user.customerID));

                    printf("User Status: %s\n",user.status);
                    printf("User Status location: %lu\n\n",user_status_location);

                    printf("User Income Class: %d\n",user.incomeclass);
                    printf("User Income Class location: %lu\n",income_class_location);

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
                    printf("Breakpoint location: %lu\n",breakpoint_location);

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
                            printf("Location line #: %lu\n",meter_size_location[i]);
                            printf(lastmeter_r_prefix"%d: %.2f\n",i+1, user.meter_reading[i]); 
                            printf("Location line #: %lu",last_meter_r_location[i]);
                                
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
                if (premisesamt == 5 && user_has_email == 1) // Has maximum premises amount and no email
                {
                    printf("(1) - Income Class\n"text_red_start"(2) - Email - customer has no login account"text_color_end"\n(3) - First Name\n(4) - Last name\n(5) - Meter Size\n(6) - Meter Reading\n(7) - All Fields\n"text_red_start"(8) - Add Premises\n"text_color_end);
                }
                else if (premisesamt == 5 && user_has_email != 1) // Has maximum premises and has email
                {
                    printf("(1) - Income Class\n(2) - Email\n(3) - First Name\n(4) - Last name\n(5) - Meter Size\n(6) - Meter Reading\n(7) - All Fields\n"text_red_start"(8) - Add Premises\n"text_color_end);
                }
                else if(user_has_email != 1 && premisesamt > 0) // If user has email linked to ID/account and they own premises, do:
                {
                    printf("(1) - Income Class\n(2) - Email\n(3) - First Name\n(4) - Last name\n(5) - Meter Size\n(6) - Meter Reading\n(7) - All Fields\n(8) - Add Premises\n");
                }
                else if(user_has_email == 1 && premisesamt > 0) //Has no email but has premises
                {
                    printf("(1) - Income Class\n"text_red_start"(2) - Email - customer has no login account"text_color_end"\n(3) - First Name\n(4) - Last name\n(5) - Meter Size\n(6) - Meter Reading\n(7) - All Fields\n(8) - Add Premises\n");
                }
                else if (user_has_email != 1 && premisesamt == 0) //Has email but no premises
                {
                    printf("(1) - Income Class\n(2) - Email\n(3) - First Name\n(4) - Last name\n(5) - All Fields\n(6) - Add Premises\n");
                }
                else if (user_has_email == 1 && premisesamt == 0) //Has no email and no premises
                {
                    printf("(1) - Income Class\n"text_red_start"(2) - Email - customer has no login account"text_color_end"\n(3) - First Name\n(4) - Last name\n(5) - All Fields\n(6) - Add Premises\n");
                }
                
                int choice;
                int a;
                
                printf("(0) - Return To Actions Menu\n");
                
                //Getting users choice
                scanf(" %d",&choice);
                
                clear_stringarray(strtemp);
                
                //Exit to action menu condition
                if(choice == 0)
                {
                    goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                }
                
                //CHANGING INCOME CLASS
                else if (choice == 1)
                {
                    //Getting new name
                    printf(underline_start"\nEnter new income class (1 - 5):\n"underline_end);
                    scanf(" %d",&user.incomeclass);

                    while(user.incomeclass != 1 && user.incomeclass != 2 && user.incomeclass != 3 && user.incomeclass != 4 && user.incomeclass != 5)
                    {
                        printf(bold_start"\n-income class invalid-\n"bold_end);
                        printf(underline_start"\nEnter valid new income class (1 - 5):\n"underline_end);
                        scanf(" %d",&user.incomeclass);
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
                                if(current_line == income_class_location)
                                {
                                    snprintf(strtemp,max_e_length,income_class_prefix"%d\n",user.incomeclass);
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
                                printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                audit_editcustomer(1,user.customerID);
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
                
                //CHANGING EMAIL
                else if(choice == 2)
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
                                    printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                    audit_editcustomer(2,user.customerID);
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
                else if (choice == 3)
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
                                printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                audit_editcustomer(3,user.customerID);
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
                else if (choice == 4)
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
                                printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                audit_editcustomer(4,user.customerID);
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
                else if (premisesamt > 0 && choice == 5)
                {
                    i = 0;
                    printf(underline_start"\nWhat meter # would you like to change?\n"underline_end);
                    scanf("%d",&choice);

                    //Ensuring user can't edit meter that customer dosen't have
                    while(choice > premisesamt || choice < 1 || strstr(user.premisesID[choice-1],surrender_prefix)!=NULL)
                    {
                        printf(bold_start"\n-Customer owns no such meter #-\n"bold_end);
                        printf(underline_start"\nEnter VALID meter #:\n"underline_end);
                        scanf("%1d",&choice);
                    }

                    printf(underline_start"\nEnter New Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,choice);
                    scanf("%3d",&user.meter_size[i]);
    
                    //Ensuring user can only select one of 3 options available
                    //Meter size 1 - 150
                    //Meter size 2 - 30
                    //Meter size 3 - 15
                    while(user.meter_size[i] != meter1 && user.meter_size[i] != meter2 && user.meter_size[i] != meter3)
                    {
                        printf(bold_start"\n-Invalid meter size-\n"bold_end);
                        printf(underline_start"\nEnter VALID Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,i+1);
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
                                printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                audit_editcustomer(5,user.customerID);
                            }
                        }
                    }
                    else if(send_back_variable == 'X' || send_back_variable == 'X')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }

                }
                
                //CHANGING METER READING
                else if (premisesamt > 0 && choice == 6)
                {
                    i = 0;
                    printf(underline_start"\nWhat meter reading # would you like to change?\n"underline_end);
                    scanf("%d",&choice);

                    //Ensuring user can't edit meter that customer dosen't haves
                    while(choice > premisesamt || choice < 1 || strstr(user.premisesID[choice-1],surrender_prefix)!=NULL)
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
                                printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                audit_editcustomer(6,user.customerID);
                            }
                        }
                    }
                    else if(send_back_variable == 'X' || send_back_variable == 'x')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }

                }
                
                //CHANGING ALL FIELDS
                else if (premisesamt > 0 && choice == 7 || premisesamt == 0 && choice == 5)
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
                            printf(underline_start"\nEnter New Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,i+1);
                            scanf("%d",&user.meter_size[i]);
            
                            //Ensuring user can only select one of 3 options available
                            //Meter size 1 - 150
                            //Meter size 2 - 30
                            //Meter size 3 - 15
                            while(user.meter_size[i] != meter1 && user.meter_size[i] != meter2 && user.meter_size[i] != meter3)
                            {
                                printf(bold_start"\n-Invalid meter size-\n"bold_end);
                                printf(underline_start"\nEnter VALID Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,i+1);
                                scanf("%d",&user.meter_size[i]);
                            }

                            char reading_temp_hold[max_length];
                            printf(underline_start"\nEnter Initial Meter Reading #%d:\n"underline_end,i+1);
                            scanf(" %s",reading_temp_hold);
    
                            while(strhchar(reading_temp_hold) != 1 || atof(reading_temp_hold)<0)
                            {
                                if(atof(reading_temp_hold)<0)
                                {
                                    printf(bold_start"\n-Reading cannot be negative-\n"bold_end);
                                    printf(underline_start"\nEnter VALID Initial Meter Reading #%d:\n"underline_end,i+1);
                                    scanf(" %s",reading_temp_hold);
                                    continue;
                                }
                                
                                printf(bold_start"\n-Reading can only be a number-\n"bold_end);
                                printf(underline_start"\nEnter VALID Initial Meter Reading #%d:\n"underline_end,i+1);
                                scanf(" %s",reading_temp_hold);
                            }
                            user.meter_reading[i] = atof(reading_temp_hold);
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
                                    fclose(temp_pointer);
                                    fclose(loginpointer);
                                }
                                else
                                {
                                    printf(file_open_error);
                                }

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
                                    printf(bold_start"\n-Records Modified Successfully-\n"bold_end);
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
                                printf("Premises Amt: %d\n\n",premisesamt-surrendered_premises_amt);
                                
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
                                    printf("\nMeter size %d line #: %lu\n",i+1,meter_size_location[i]);
                                    printf("Last Meter Reading %d line #: %lu",i+1,last_meter_r_location[i]);
                                }
                                printf("\n==================================\n\n");
                            }
                            
                            //Editing all other fields
                            clear_stringarray(str);
                            int fault_check;
                            
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
                                fclose(temp_pointer);
                                fclose(customerdbpointer);

                            }
                            else
                            {
                                printf(file_open_error);
                            }



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
                                    printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                    audit_editcustomer(7,user.customerID);
                                }
                            }

                        }
                    }
                    else if (send_back_variable == 'X' || send_back_variable == 'x')
                    {
                        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
                    }
                }
                
                //ADDING PREMISES
                else if (premisesamt > 0 && choice == 8 || premisesamt == 0 && choice == 6)
                {
                    if (premisesamt-surrendered_premises_amt < 5)
                    {
                        int premises_to_add;
                        system(terminal_clear_string); //Clears command line UI
                        printf(bold_start"================== ADDING PREMISES ==================\n\n"bold_end); //Outputting file header
                        printf("CUSTOMER NAME: %s %s\n",user.firstname,user.lastname);
                        printf("CUSTOMER ID: %s\n\n",user.customerID);

                        printf(underline_start"How many Premises would you like to add (Max 5 per customer): \n"underline_end);
                        scanf(" %d", &premises_to_add);
                        
                        //Ensuring user cannot select more than 5 premises to add
                        while(premises_to_add+(premisesamt-surrendered_premises_amt)>5)
                        {
                            printf(bold_start"\nCustomer only has %d slot/s available\n\n"bold_end,5-(premisesamt-surrendered_premises_amt));
                            printf(underline_start"How many Premises would you like to add (Max 5 per customer): \n"underline_end);
                            scanf(" %d", &premises_to_add);
                        }
                            
                        //Gettings Premises details 
                        char temp_premisesID_hold[5][max_e_length] = {0}; // Variable used to store premises numbers already entered
                        int id_record_exists = 1; // Default to false 
                        int redo_gettingID;
                        for(int i = 0; i < premises_to_add; i++)          // and check to ensure current premises IDs  being entered, dont match past ones
                        {
                            system(terminal_clear_string); //Clears command line UI
                            printf(bold_start"================== ADDING PREMISES ==================\n\n"bold_end); //Outputting file header
                            printf("CUSTOMER NAME: %s %s\n",user.firstname,user.lastname);
                            printf("CUSTOMER ID: %s\n",user.customerID);

                            if(redo_gettingID == 0)
                            {
                                printf(bold_start"\n-Premises ID already entered-\n"bold_end);
                                printf(underline_start"\nEnter NEW premises ID (7 digits) #%d: \n"underline_end,i+1);
                                scanf(" %s",strtemp);
                            }
                            else
                            {
                                printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                                scanf(" %s",strtemp);
                            }
                            
                            redo_gettingID = 1; // Default to false

                            //Ensuring Premises ID entered is 7 digits long and contains only numbers
                            while(strlen(strtemp)!=7 || strhchar(strtemp) != 1)
                            {
                                if(strhchar(strtemp) != 1)
                                {
                                    printf(bold_start"\n-Premises ID cannot contain letters\n"bold_end);
                                    printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                                    scanf(" %s",strtemp);
                                    continue;
                                }
                                printf(bold_start"\n-Max & Min ID size is (7) digits-\n"bold_end);
                                printf(underline_start"\nEnter premises ID (7 digits) #%d: \n"underline_end,i+1);
                                scanf(" %s",strtemp);
                            }
                
                            
                            //Ensuring no dupliciate Premises ID can be made
                            while(duplicate_check(strtemp,customerdatabase,premisesid_prefix) != 2)
                            {
                                if(get_premises_status(strtemp)!=1)
                                // ID found was not surrendered, do:
                                {
                                    printf(bold_start"\n-Premises In Use-\n\n"bold_end);
                                    printf(underline_start"Enter New Premises ID #%d: \n"underline_end,i+1);
                                    scanf(" %s",strtemp);
                                }
                                else
                                {
                                    id_record_exists = 0; // Premises ID is available
                                    break; // Premises ID is available as the other instance found is marked as surrendered
                                }
                            }
                            
                            strcpy(temp_premisesID_hold[i],strtemp);
                            
                            if(debug == 0)
                            {
                                printf("\nPrevious user entered IDS: ");
                                for(int c = 0; c < i; c++)
                                {
                                    printf("%s",temp_premisesID_hold[c]);
                                } 
                                printf("\nCurrent ID: %s\n",strtemp);
                                printf("\nCurrent index (i): %d\n",i);
                            }
                            
                            if(i > 0) //If we're not entering our first premises
                            {
                                for(int a = 0; a<i; a++)
                                {
                                    if(strcmp(temp_premisesID_hold[a],strtemp) == 0) // If a matching premises is found
                                    {
                                        redo_gettingID = 0; //Set to true
                                        break;
                                    }
                                }
                            }
                            if(redo_gettingID == 0)
                            {
                                i--; //decrement i such that we can re-enter premises id which wouldve been stored at current index 
                                continue;
                            }
                            else
                            {
                                strcpy(user.premisesID[i],strtemp);
                                
                                char choice;
                                int temp_mtr_size=0;
                                float temp_lmtr_reading=0;
                                int temp_premises_amt = 0;

                                //Using previous meter size from file
                                if(id_record_exists != 1)
                                {
                                    fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                                    found_ID = 1; //Default to false
                                    found_breakpoint = 1; //Default to false
                                    
                                    if(customerdbpointer != NULL)
                                    {
        
                                        while(fgets(str,max_e_length,customerdbpointer)!= NULL && found_breakpoint != 0)
                                        {
                                            if(strstr(str,premisesid_prefix) != NULL && strstr(str,strtemp2) != NULL) // If id is found
                                            {
                                                found_ID = 0;
                                            }
                                            if(strstr(str,data_breakpoint) != NULL && found_ID == 0)
                                            {
                                                found_breakpoint = 0;
                                                break;
                                            }

                                            //Getting specific premises info
                                            if(found_ID == 0)
                                            {
                                                if(strstr(str,premisesid_prefix))
                                                {
                                                    temp_premises_amt++;
                                                }
                                                else if(strstr(str,metersize_prefix))
                                                {
                                                    snprintf(strtemp2,max_e_length,metersize_prefix"%d: ",temp_premises_amt);
                                                    remove_prefix(str,strtemp2);
                                                    temp_mtr_size = atoi(str);
                                                }
                                                else if(strstr(str,lastmeter_r_prefix))
                                                {
                                                    snprintf(strtemp2,max_e_length,lastmeter_r_prefix"%d: ",temp_premises_amt);
                                                    remove_prefix(str,strtemp2);
                                                    temp_lmtr_reading = atof(str);
                                                }
                                            }
                                        }
                                        fclose(customerdbpointer);

                                        printf("\nPrevious meter size found for premises ID (%s)\nWould you like to use it? (Y) Yes OR (N) No\n",user.premisesID[i]);
                                        scanf(" %c", &choice);

                                        while(choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n')
                                        {
                                            printf(bold_start"\n-Invalid Selection-\n"bold_end);
                                            printf("Would you like to use it? (Y) Yes OR (N) No\n");
                                            scanf(" %c", &choice);
                                        }

                                        if(choice == 'Y' || choice == 'y')
                                        {
                                            user.meter_size[i] = temp_mtr_size;  
                                        }
                                    }
                                    else
                                    {
                                        printf(file_open_error);
                                    }
                                }

                                // Getting new size
                                if(id_record_exists == 1 || choice == 'N' || choice == 'n')
                                {
                                    printf(underline_start"\nEnter Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,i+1);
                                    scanf("%d",&user.meter_size[i]);

                                    //Ensuring user can only select one of 3 options available
                                    //Meter size 1 - 150
                                    //Meter size 2 - 30
                                    //Meter size 3 - 15
                                    while(user.meter_size[i] != meter1 && user.meter_size[i] != meter2 && user.meter_size[i] != meter3)
                                    {
                                        printf(bold_start"\n-Invalid meter size-\n"bold_end);
                                        printf(underline_start"\nEnter VALID Meter size (%dmm - %dmm - %dmm) #%d:\n"underline_end,meter3,meter2,meter1,i+1);
                                        scanf("%d",&user.meter_size[i]);
                                    }
                                }
                                
                                //Using previous meter reading from file
                                if(id_record_exists != 1)
                                {
                                    printf("\nPrevious meter reading found for premises ID (%s)\nWould you like to use it? (Y) Yes OR (N) No\n",user.premisesID[i]);
                                    scanf(" %c", &choice);

                                    while(choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n')
                                    {
                                        printf(bold_start"\n-Invalid Selection-\n"bold_end);
                                        printf("Would you like to use it? (Y) Yes OR (N) No\n");
                                        scanf(" %c", &choice);
                                    }

                                    if(choice == 'Y' || choice == 'y')
                                    {
                                        user.meter_reading[i] = temp_lmtr_reading;  
                                    }
                                }

                                // Getting new reading
                                char reading_temp_hold[max_length] = {0};
                                if(id_record_exists == 1 || choice == 'N' || choice == 'n')
                                {
                                    printf(underline_start"\nEnter Initial Meter Reading #%d:\n"underline_end,i+1);
                                    scanf(" %s",reading_temp_hold);

                                    while(strhchar(reading_temp_hold) != 1 || atof(reading_temp_hold)<0)
                                    {
                                        if(atof(reading_temp_hold)<0)
                                        {
                                            printf(bold_start"\n-Reading cannot be negative-\n"bold_end);
                                            printf(underline_start"\nEnter VALID Initial Meter Reading #%d:\n"underline_end,i+1);
                                            scanf(" %s",reading_temp_hold);
                                            continue;
                                        }
                                        printf(bold_start"\n-Reading can only be a number-\n"bold_end);
                                        printf(underline_start"\nEnter VALID Initial Meter Reading #%d:\n"underline_end,i+1);
                                        scanf(" %s",reading_temp_hold);
                                    }

                                    user.meter_reading[i] = atof(reading_temp_hold);
                                }
                            }
                        }
                        
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
                                    if(current_line == breakpoint_location)
                                    {
                                        for(a = 0; a<premises_to_add; a++)
                                        {
                                            clear_stringarray(strtemp);
                                            fputs("\n",temp_pointer);
                                            snprintf(strtemp,max_e_length,premisesid_prefix"%d: %s\n",a+1,user.premisesID[a]);
                                            fputs(strtemp,temp_pointer);
                                            
                                            snprintf(strtemp,max_e_length,metersize_prefix"%d: %d\n",a+1,user.meter_size[a]);
                                            fputs(strtemp,temp_pointer);
                                            
                                            snprintf(strtemp,max_e_length,lastmeter_r_prefix"%d: %.2f\n",a+1,user.meter_reading[a]);
                                            fputs(strtemp,temp_pointer);
                                            
                                        }
                                        fputs(data_breakpoint,temp_pointer);
                                        fputs("\n",temp_pointer);
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
                                    printf(bold_start"\n-Record Modified Successfully-\n\n"bold_end);
                                    audit_editcustomer(11,user.customerID);
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
                    else
                    {
                      printf(text_red_start"\nCustomer CANNOT have more than 5 premises\n\n"text_color_end);  
                    }
                }
            }
            
            break;
            
        case '3': // View customer/s
            
            customerdbpointer = fopen(customerdatabase, "r"); // attempts to open file in READ mode (r)
            loginpointer = fopen(loginfile,"r");
            unsigned customeramt = 0;

            if(customerdbpointer != NULL && loginpointer != NULL) // If File open attempt was successful
            {
                fseek(customerdbpointer,strlen("CUSTOMER DATABASE"),SEEK_SET); //Setting file pointer past header

                printf(underline_start"View all customers (A) or specific customer (P):\n"underline_end);
                scanf(" %c", &customers_to_view);
                
                while(customers_to_view != 'A' && customers_to_view != 'a' && customers_to_view != 'P' && customers_to_view != 'p')
                {
                    system(terminal_clear_string); // Clears command line UI
                    printf(bold_start"-Invalid Selection-\n"bold_end);
                    printf(underline_start"View all customers (A) or specific customer (P):\n"underline_end);
                    scanf(" %c", &customers_to_view);
                }

                /// OUTPUTTING ALL CUSTOMER DATA 
                if(customers_to_view == 'A' || customers_to_view == 'a')
                {

                    system(terminal_clear_string); //Clears command line 

                    clear_stringarray(str);
                    snprintf(str,max_e_length,"\n     ID           STATUS    INCOMECLASS         EMAIL                       FIRST NAME         LASTNAME       # of PREMISES OWNED\n");
                    int footer_line_width = strlen(str);
                    char header_border[max_length];

                    for(i = 0; i<(footer_line_width-strlen(" CUSTOMER DATABASE "))/2; i++)
                    {
                        header_border[i] = '=';
                    }
                    header_border[i] = '\0';

                    printf(bold_start"%s CUSTOMER DATABASE %s"bold_end,header_border,header_border); //Outputting file header
                    printf("%s",str);
                    
                    clear_stringarray(str);
                    for(i = 0; i<footer_line_width; i++)
                    {
                        str[i] = '=';
                    }
                    printf("%s",str);
                    printf("\n");

                    if(customerdbpointer!=NULL)
                    {
                        while(feof(customerdbpointer) == 0)
                        //Loop continues until end of file is reached
                        {
                            // Printing Each customer data
                            if(customeramt>0)
                            {
                                int fname_length = strlen(user.firstname);
                                int lname_length = strlen(user.lastname);
                                int email_length = strlen(user.email);

                                int space_needed_name = 15 - fname_length;
                                int space_needed_email = 35 - email_length;
                                int space_needed_premises = 25 - lname_length;
                                char spaces_name[max_length];
                                char spaces_email[max_length];
                                char spaces_premises[max_length];
                            
                                for(i = 0; i < space_needed_name; i++)
                                {
                                    strcpy(&spaces_name[i]," ");
                                }
                                
                                for(i = 0; i < space_needed_email; i++)
                                {
                                    strcpy(&spaces_email[i]," ");
                                }
                                
                                for(i = 0; i < space_needed_premises; i++)
                                {
                                    strcpy(&spaces_premises[i]," ");
                                }
                                
                                if(strcmp(user.status,"ARCHIVED")==0)
                                {
                                    printf(text_red_start);
                                }
                                clear_stringarray(str);
                                
                                snprintf(str,max_e_length,"  %s  \t  %s \t %d \t %s %s %s %s %s %s %d\n",user.customerID,user.status,user.incomeclass,user.email,spaces_email,user.firstname,spaces_name,user.lastname,spaces_premises,premisesamt);
                                printf("%s",str);
                                if(strcmp(user.status,"ARCHIVED")==0)
                                {
                                    printf(text_color_end);
                                }
                                
                                //Printing line between each customer record
                                clear_stringarray(str);
                                for(i = 0; i<footer_line_width; i++)
                                {
                                    str[i] = '-';
                                }
                                printf("%s",str);
                                printf("\n");

                            }
                            customeramt++; //Increment customer count

                            //Getting info from customer database
                            found_ID = 1; //Default to false
                            found_breakpoint = 1; //Default to false
                            premisesamt = 0;

                            while(fgets(str,max_e_length,customerdbpointer) != NULL && found_breakpoint != 0)
                            {

                                if(strstr(str,id_prefix)!=NULL)
                                {
                                    found_ID = 0;
                                    remove_prefix(str,id_prefix);
                                    strsanitize(str,0);
                                    strcpy(user.customerID,str);
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
                                //Getting account status
                                else if(found_ID == 0 && strstr(str,status_prefix) != NULL)
                                {
                                    strcpy(user.status,str);
                                    user_status_location = current_line;
                                }
                                //Getting income class
                                else if(found_ID == 0 && strstr(str,income_class_prefix) != NULL)
                                {
                                    clear_stringarray(strtemp);
                                    strcpy(strtemp,str);
                                    remove_prefix(strtemp,income_class_prefix);
                                    user.incomeclass = atoi(strtemp); // converts income class from str to int
                                    income_class_location = current_line;
                                }
                                else if(found_ID == 0 && strstr(str,premisesid_prefix) != NULL)
                                {
                                    if(strstr(str,surrender_prefix)==NULL) // Only increment premises count if premises being looked at isnt surrendered
                                    {
                                        premisesamt++;  
                                    }
                                }

                            }
                            
                            current_line = 1;

                            found_ID = 1; //Default to false
                            found_breakpoint = 1; //Default to false

                            //Getting email
                            clear_stringarray(user.email);
                            while(fgets(str,max_e_length,loginpointer) != NULL && found_breakpoint != 0)
                            {
                                if(strstr(str,user.customerID)!=NULL) //If id is found
                                {
                                    found_ID = 0;
                                }
                                else if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
                                {
                                    found_breakpoint = 0;
                                    break;
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
                            
                            //Removes prefix such as "UserID: " from string/s
                            if(strlen(user.email)>0)
                            {
                                remove_prefix(user.email,email_prefix);
                            }
                            else
                            {
                                strcpy(user.email,"NONE FOUND");
                            }
                            remove_prefix(user.firstname,fname_prefix);
                            remove_prefix(user.lastname,lname_prefix);
                            remove_prefix(user.status,status_prefix);

                            //Removes "\n" from string/s
                            strsanitize(user.email,0);
                            strsanitize(user.firstname,0);
                            strsanitize(user.lastname,0);
                            strsanitize(user.status,0);

                            if(debug == 0)
                            {
                                printf("\n=========== DEBUG DATA ===========\n");
                                printf("UserID (from user): %s\n",user.customerID);
                                printf("UserID strlen: %lu\n",strlen(user.customerID));
            
                                printf("User Status: %s\n",user.status);
                                printf("User Status location: %lu\n\n",user_status_location);
            
                                printf("User Income Class: %d\n",user.incomeclass);
                                printf("User Income Class location: %lu\n",income_class_location);
            
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

                                printf("==================================\n");
                            }
                                                    
                        }

                        fclose(customerdbpointer);
                        fclose(loginpointer);
                        //Printing line between each customer record
                        clear_stringarray(str);
                        for(i = 0; i<footer_line_width; i++)
                        {
                            str[i] = '=';
                        }
                        printf("%s",str);
                        printf("\n");
                        printf(bold_start"CUSTOMER TOTAL #: %d\n"bold_end,customeramt-1);
                        printf("%s",str);
                        printf("\n");
                    }
                }
                
                /// OUTPUTTING SPECIFIC CUSTOMER DATA
                else if(customers_to_view == 'P' || customers_to_view == 'p')
                {
                    printf(underline_start"Enter customerID to lookup:\n"underline_end);
                    scanf(" %s", user.customerID);

                    //Sanitizing values ( removing new line values, carriages, white space and other unwanted characters)
                    strsanitize(user.customerID,0);
                    
                    while(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2 || strhchar(user.customerID)==0)
                    //while duplicate data not found (information provided dosent exist in file checked)
                    // do:
                    {
                        if(strhchar(user.customerID)==0)
                        {
                            printf(bold_start"\n-ID cannot have letters-\n\n"bold_end);
                            printf(underline_start"Enter new Customer ID:\n"underline_end);
                            scanf(" %s", user.customerID);
                            continue;
                        }
                        printf(bold_start"\n-No Such Customer Exists-\n\n"bold_end);
                        printf(underline_start"Enter new ID to lookup:\n"underline_end);
                        scanf(" %s", user.customerID);
                        strsanitize(user.customerID,0);
                    }
                    
                    system(terminal_clear_string); // Clears command line UI
                    
                    printf(bold_start"================== CUSTOMER DATABASE ==================\n\n"bold_end); //Outputting file header
                                        
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
                        fclose(loginpointer);
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
                            printf(text_color_end);
                            found_breakpoint = 0;
                        }

                        if(strstr(str,premisesid_prefix) != NULL && found_ID == 0 && found_breakpoint != 0)
                        {
                            if(strstr(str,surrender_prefix)==NULL) // Only increment premises count if premises being looked at isnt surrendered
                            {
                                premisesamt++;
                            }
                        }
                        
                        if(found_ID == 0 && strstr(str,data_breakpoint) == NULL)
                        {
                            //Displaying archived customer status as RED
                            if(strstr(str,status_prefix"ARCHIVED")!=NULL)
                            {
                                printf(text_red_start"%s"text_color_end,str);
                            }
                            else if(strstr(str,balance_overdue_prefix)!=NULL)
                            {
                                printf(text_red_start"%s"text_color_end,str);
                            }
                            else if(strstr(str,surrender_prefix)!=NULL && strstr(str,premisesid_prefix)!=NULL)
                            {

                                //Removing surrender prefix from string
                                int t = strlen(str);
                                for(int i = 1 ; i<5; i++)
                                {
                                    str[t-i] = '\0'; 
                                }

                                strcat(str," - Surrendered -");
                                printf(text_red_start"%s\n",str);
                            }
                            else if(strstr(str,premisesid_prefix)!=NULL && strstr(str,surrender_prefix)==NULL)
                            {
                                printf(text_color_end);
                                printf("%s",str);
                            }
                            else
                            {
                                printf("%s",str);
                            }
                        }
                        
                    }
                    fclose(customerdbpointer);
                    
                    printf(bold_start"\n=======================================================\n"bold_end);
                    printf(underline_start"PREMISES OWNED %d\n"underline_end,premisesamt);
                    printf(bold_start"=======================================================\n"bold_end);
                    printf(underline_start"INCOME CLASS\n"underline_end);
                    printf("1 - Low: daily usage of up to %d litres\n",Low);
                    printf("2 - Low Medium: daily usage of up to %d litres\n",Low_Medium);
                    printf("3 - Medium: daily usage of up to %d litres\n",Medium);
                    printf("4 - Medium High: daily usage of up to %d litres\n",Medium_High);
                    printf("5 - High: daily usage of up to %d litres\n",High);
                    printf(bold_start"=======================================================\n"bold_end);
                    
                }

                if(debug == 0)
                {           
                    printf(bold_start"=========== DEBUG DATA ==========="bold_end);
                    
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
                    printf(bold_start"==================================\n\n"bold_end);
                    
                }

                break;
            }
            else
            {
                printf(file_open_error);
                break;
            }
            
            break;
        case '4': // Delete/Archive customer/s
            system(terminal_clear_string);
            char reason[max_e_length];
            customerdbpointer = fopen(customerdatabase, "r"); // Attempts to open file in reading mode (r)
        
            printf(bold_start"================== CUSTOMER DELETION/ARCHIVING ==================\n\n"bold_end); //Outputting file header
            printf(underline_start"Enter ID of customer to be deleted\n"underline_end);
            printf(text_red_start"NOTE: THIS CANNOT BE UNDONE\n"text_color_end);
            fflush(stdin);
            scanf(" %s",user.customerID);

            //Checking if ID provided exists and isnt archived already
            while(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2 || get_user_status(user.customerID) != 0 || strhchar(user.customerID)==0)
            //while duplicate data not found (information provided dosent exist in file checked)
            // do:
            {
                if(strhchar(user.customerID)==0)
                {
                    printf(bold_start"\n-ID cannot have letters-\n\n"bold_end);
                    printf(underline_start"Enter new Customer ID:\n"underline_end);
                    scanf(" %s", user.customerID);
                    continue;
                }
                else if(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2)
                {
                    printf(bold_start"\n-No Such Customer Exists-\n\n"bold_end);
                }
                else if (get_user_status(user.customerID) != 0)
                {
                    printf(bold_start"\n-Customer Is Already Archived-\n\n"bold_end);
                }

                printf(underline_start"Enter new ID to lookup:\n"underline_end);
                scanf(" %s", user.customerID);
                strsanitize(user.customerID,0);
            } 

            printf(underline_start"\nEnter reason for deletion/archiving\n"underline_end bold_start"This will be recorded in the customer database\n"bold_end);
            fflush(stdin);
            scanf(" "); // catching newline character (\n).
            fgets(reason,max_e_length,stdin);

            while(strlen(reason)<2)
            {
                printf(bold_start"\n-Please provide VALID reason for deletion-\n\n"bold_end);
                printf(underline_start"Enter reason for deletion/archiving\n"underline_end bold_start"This will be recorded in the customer database\n"bold_end);
                fflush(stdin);
                getchar(); // catching newline character (\n).
                scanf(" ");
                fgets(reason,max_e_length,stdin);
            }
            
            system(terminal_clear_string);
            
            printf(bold_start"================== CUSTOMER DELETION/ARCHIVING ==================\n\n"bold_end); //Outputting file header


            //GETTING USER DATA
            found_breakpoint = 1; //Setting found breakpoint ";" back to false
            found_ID = 1; //Setting found ID back to false
            int i = 0;
            current_line = 1;

            if(customerdbpointer!=NULL)
            {
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
                    
                    if(found_ID == 0 && found_breakpoint != 0)
                    {
                        if(strstr(str,surrender_prefix)!=NULL)
                        {
                            printf(text_red_start);
                            printf("%s",str);
                        }
                        else
                        {
                            printf("%s",str);
                        }
                        
                        if(strstr(str,lastmeter_r_prefix)!=NULL)
                        {
                            printf(text_color_end);
                        }
                    }

                    
                    if(found_ID == 0)
                    {
                        //Getting account status
                        if(strstr(str,status_prefix) != NULL)
                        {
                            remove_prefix(str,status_prefix);
                            strcpy(user.status,str);
                            user_status_location = current_line;
                        }
                    
                        //Getting premises info
                        char final_prefix[max_length];
                        char i_as_string[1]; // i + terminating char "\0"
                
                        if(strstr(str, premisesid_prefix) != NULL)
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
                                
                                
                                //Removing surrender_prefix if found
                                if(strstr(ptr,surrender_prefix)!=NULL)
                                {
                                    *(ptr+7) = '\0';
                                }
                                
                                strcpy(user.premisesID[i],ptr);
                                ptr = NULL;
                            }
                            premisesID_location[i] = current_line; // Saves line number where premises id was found
                            premisesamt++;
                            i++;
                        }
                    }
                    current_line++;
                }
            }
            
            printf(bold_start"\n=================================================================\n"bold_end);
            printf(bold_start"Premises in "text_red_start"red"text_color_end" are surrendered, and the remaing premises will \nalso be after customer deletions"bold_end);
            printf(bold_start"\n=================================================================\n"bold_end);
            
            //CHANGING CUSTOMER STATUS & ADDING REASON
            int login_access;
            int is_customer_new;
            clear_stringarray(strtemp);
            clear_stringarray(filename_temp);

            fflush(stdin);
            printf(bold_start"\nConfirm deactivation request for customer (%s) ? (Y) Yes OR (X) no\nYou will be taken back to admin actions menu after selection\n"text_red_start"THIS ACTION CANNOT BE UNDONE\n"text_color_end bold_end,user.customerID);
            scanf(" %c", &send_back_variable);

            if(send_back_variable == 'Y' || send_back_variable == 'y')
            {
                system(terminal_clear_string);

                printf(bold_start"================== CUSTOMER DELETION/ARCHIVING ==================\n"bold_end); //Outputting file header
                
                printf(bold_start"\nEnter Admin Credentials to Proceed with action\n"bold_end);
                printf(underline_start"\nEnter Email\n"underline_end);
                scanf(" %s",user.email);

                printf(underline_start"\nEnter Password\n"underline_end);
                fflush(stdin);
                if(terminal == 'L' || terminal == 'l')
                {
                    scanf(" %s",user.password);
                }
                else if(terminal == 'W' || terminal == 'w')
                {
                   scanfpassword(user.password); 
                }
                
                login_access = user_login(user.email,user.password,strtemp,&is_customer_new);

                if(login_access == 2) //If admin login successful, archive user.
                {
                    clear_stringarray(filename_temp);
                    clear_stringarray(str);
                
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
                        i = 0;
                        current_line = 1;
                        while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                        {
                            if(current_line == user_status_location)
                            {
                                time_t t = time(NULL);
                                struct tm date = *localtime(&t);

                                snprintf(strtemp,max_e_length,status_prefix"ARCHIVED\n");
                                fputs(strtemp,temp_pointer);
                                snprintf(strtemp,max_e_length-max_length,reason_prefix"%s",reason);
                                fputs(strtemp,temp_pointer);
                                snprintf(strtemp,max_e_length,date_archived_prefix"%02d/%02d/%04d\n",date.tm_mday,date.tm_mon+1,date.tm_year+1900);
                                fputs(strtemp,temp_pointer);
                            }
                            else if(current_line == premisesID_location[i])
                            // Checks if lines match and if premises isnt already surrendered
                            {
                                snprintf(strtemp,max_e_length,premisesid_prefix"%d: %s%s\n",i+1,user.premisesID[i],surrender_prefix);
                                fputs(strtemp,temp_pointer);
                                i++;
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
                            printf(bold_start"\n\n-Customer Archived-\n"bold_end);
                            audit_editcustomer(8,user.customerID);
                        }
                    }
                    else
                    {
                        printf(file_open_error);
                    }
                }
                else
                {
                    printf(bold_start"\nINVALID CREDENTIALS - Customer status change aborted\n"bold_end);
                }
                printf(bold_start"\n=================================================================\n"bold_end);
            }
            else if(send_back_variable == 'X' || send_back_variable == 'x')
            {
                goto jump_admin_actions; // Jumps code back to specified point if logic returns true
            }
            break;

        case '5': // Generate Bill customer/s
            user_has_email = 1; //1 for false - 0 for true
            premisesamt = 0; // Clear any value stored from previous actions
            
            printf(bold_start"================== BILLING TERMINAL ==================\n\n"bold_end); //Outputting file header
            printf(underline_start"Enter ID of customer who's bill you'd like to generate:\n"underline_end);
            fflush(stdin);
            scanf(" %s", user.customerID);
            
            //Validating customer ID
            while(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2 || get_user_status(user.customerID) != 0 || strhchar(user.customerID)==0)
            //while duplicate data not found (information provided dosent exist in file checked)
            // do:
            {
                if(strhchar(user.customerID)==0)
                {
                    printf(bold_start"\n-ID cannot have letters-\n\n"bold_end);
                    printf(underline_start"Enter new Customer ID:\n"underline_end);
                    scanf(" %s", user.customerID);
                    continue;
                }
                else if(duplicate_check(user.customerID,customerdatabase,id_prefix) == 2)
                {
                    printf(bold_start"\n-No Such Customer Exists-\n\n"bold_end);
                }
                else if(get_user_status(user.customerID) != 0)
                {
                    printf(bold_start"\n-This customer account is archived-\n\n"bold_end);
                    printf(underline_start"Enter ID of ACTIVE customer who's bill you'd like to generate:\n"underline_end);
                    fflush(stdin);
                    scanf(" %s", user.customerID);
                    continue;
                }
                
                printf(underline_start"Enter VALID ID of customer who's bill you'd like to generate:\n"underline_end);
                fflush(stdin);
                scanf(" %s", user.customerID);
            }

            // attempts to open files in READ mode (r)
            customerdbpointer = fopen(customerdatabase, "r");

            found_breakpoint = 1; //Setting found breakpoint ";" back to false
            found_ID = 1; //Setting found ID back to false   
            
            if(customerdbpointer != NULL && loginpointer != NULL) // If both files were open successfully
            {               
                //Getting user Data
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
                        breakpoint_location = current_line;
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
                    //Getting billing cycle amount
                    else if(found_ID == 0 && strstr(str,cycle_prefix) != NULL)
                    {
                        remove_prefix(str,cycle_prefix);
                        user.billing_cycle = atoi(str); // converts billing cycle from str to int
                        cycle_location = current_line;
                    }
                    //Getting balance overdue
                    else if(found_ID == 0 && strstr(str,balance_overdue_prefix) != NULL)
                    {
                        clear_stringarray(strtemp);
                        strcpy(strtemp,str);
                        remove_prefix(strtemp,balance_overdue_prefix);
                        user.balance_overdue_amt = atof(strtemp); // converts income class from str to float
                        balance_overdue_location = current_line;
                    }
                    //Getting account status
                    else if(found_ID == 0 && strstr(str,status_prefix) != NULL)
                    {
                        strcpy(user.status,str);
                        user_status_location = current_line;
                    }
                    //Getting income class
                    else if(found_ID == 0 && strstr(str,income_class_prefix) != NULL)
                    {
                        clear_stringarray(strtemp);
                        strcpy(strtemp,str);
                        remove_prefix(strtemp,income_class_prefix);
                        user.incomeclass = atoi(strtemp); // converts income class from str to int
                        income_class_location = current_line;
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
                remove_prefix(user.status,status_prefix);

                //Removes "\n" from string/s
                strsanitize(user.email,0);
                strsanitize(user.firstname,0);
                strsanitize(user.lastname,0);
                strsanitize(user.status,0);
                // We now have all associated user data in customer database
                
                if(strcmp(user.status,"ACTIVE") == 0) // If Account is active
                {
                    if(user.billing_cycle<2)
                    {
                        if(premisesamt>0) // If person owns premises
                        {
                            system(terminal_clear_string); // Clears command line UI

                            printf(bold_start"================== BILLING TERMINAL ==================\n\n"bold_end); //Outputting file header with user entered data
                            printf(bold_start"%s %s\n"bold_end,user.firstname ,user.lastname);
                            printf(id_prefix"%s\n",user.customerID);
                            printf(income_class_prefix"%d\n",user.incomeclass);
                            printf(balance_overdue_prefix"%.2f\n",user.balance_overdue_amt);
                            
                            if(premisesamt>0)
                            {
                                printf("\n");
                                printf(bold_start"PREMISES OWNED %d\n"bold_end,premisesamt);
                            }
                
                            //Printing premises data
                            for(int i = 0; i < premisesamt; i++)
                            {
                                printf(premisesid_prefix"%d: %s\n",i+1, &user.premisesID[i][0]);
                                printf(metersize_prefix"%d: %d\n",i+1,user.meter_size[i]);
                                printf(lastmeter_r_prefix"%d: %.2f\n\n",i+1, user.meter_reading[i]);
                            }
                            printf(bold_start"======================================================\n"bold_end);
                            
                            int premises_pick;
                            printf(underline_start"\nPick premises # to generate bill:\n"underline_end);
                            scanf(" %d" , &premises_pick);

                            while(premises_pick>premisesamt || premises_pick<1 )
                            {
                                printf(bold_start"\n-User has no such premises-\n"bold_end);
                                printf(underline_start"\nPick VALID premises # to generate bill:\n"underline_end);
                                scanf(" %d" , &premises_pick);
                            }
                            
                            time_t t = time(NULL);
                            struct tm date = *localtime(&t);
                            premises_pick--; // decrements variable, so it matches actual position of index stored

                            // CALCULATING BILL
                            int class_usage;
                            float monthly_total = 0;
                            float early_payment_reduction = 0; // Default to 0% reduction
                            float service_charge = 0;
                            float overdue_amt = user.balance_overdue_amt;
                            float total_amount_due = 0;

                            monthly_total = 0;

                            // Assigning income class values based on interger value
                            switch (user.incomeclass)
                            {
                                case 1:
                                    class_usage = Low;
                                    break;
                                case 2:
                                    class_usage = Low_Medium;
                                    break;
                                case 3:
                                    class_usage = Medium;
                                    break;
                                case 4:
                                    class_usage = Medium_High;
                                    break;
                                case 5:
                                    class_usage = High;
                                    break;
                            }

                            // Assigning service charge based on meter size
                            switch (user.meter_size[premises_pick])
                            {
                                case 15:
                                    service_charge = 1155.92;
                                    break;
                                case 30:
                                    service_charge = 6217.03;
                                    break;
                                case 150:
                                    service_charge = 39354.59;
                                    break;
                            }
                            
                            srand(time(NULL)); //Setting random seed to current time (down to the second)

                            for(int a = 0; a<30; a++)
                            {
                                monthly_total += rand()%class_usage+1; //Generates and adds random water usage based on income class
                            }

                            //Determining Early Payment Eligibility
                            // 1 for ineligible
                            // 2 for eligilble
                            if((1 + rand()%((2-1)+1)) == 2)
                            {
                                early_payment_reduction = .05; // 5% reduction 
                            }

                            float current_consumption = monthly_total - user.meter_reading[premises_pick];
                            if(current_consumption<0) // If previous meter reading is more than current
                            {
                                current_consumption  = 0;
                            }

                            float water_rate = 0;
                            float sewerage_rate = 0;

                            // Getting Water Rate and Sewerage Rate
                            if(current_consumption<=14000) // 0–14,000 litres 
                            {
                                water_rate = 149.44;
                                sewerage_rate = 172.72;
                            }
                            else if(current_consumption>=14001 && current_consumption<=27000) // 14,001–27,000 litres 
                            {
                                water_rate = 266.15;
                                sewerage_rate = 290.10;
                            }
                            else if(current_consumption>=27001 && current_consumption<=41000) // 27,001–41,000 litres 
                            {
                                water_rate = 290.10;
                                sewerage_rate = 335.06;
                            }
                            else if(current_consumption>41000) // Over 41,000 litres 
                            {
                                water_rate = 494.87;
                                sewerage_rate = 571.56;
                            }

                            float water_charge = current_consumption * water_rate;
                            float sewerage_charge = current_consumption * sewerage_rate;
                            float PAM /*Price Adjustment Mechanism*/ = .0121 * (water_charge + sewerage_charge + service_charge);
                            float x_factor = .05 * (water_charge + sewerage_charge + service_charge);
                            float k_factor = .20 * (water_charge + sewerage_charge + service_charge + PAM);
                            float total_current_charges = (water_charge + sewerage_charge + service_charge) - (x_factor + k_factor);
                            total_amount_due = (total_current_charges  - (total_current_charges*early_payment_reduction)) + user.balance_overdue_amt;

                            if(debug == 0)
                            {
                                printf(bold_start"=========== DEBUG DATA ===========\n"bold_end);
                                printf(bold_start"PREMISES #%d\n"bold_end,premises_pick);
                                printf("Previous meter reading: %.2f\n",user.meter_reading[premises_pick]);
                                printf("Current meter reading: %.2f\n",monthly_total);
                                printf("Current consumption: %.2f\n",current_consumption);
                                printf("Water Charge: %.2f\n",water_charge);
                                printf("Sewerage Charge: %.2f\n",sewerage_charge);
                                printf("Service Charge: %.2f\n",service_charge);
                                printf("PAM (Price Adjustment Mechanism): %.2f\n",PAM);
                                printf("X-Factor: %.2f\n",x_factor);
                                printf("K-Factor: %.2f\n",k_factor);
                                printf("Total Current Charges: %.2f\n",total_current_charges);
                                printf("Total Amount Due: %.2f\n",total_amount_due);
                                if(early_payment_reduction == 0)
                                {
                                    printf("Early Payment Reduction: NOT eligible\n\n");
                                }
                                else
                                {
                                    printf("Early Payment Reduction: Eligible\n\n");
                                    
                                }
                                printf(bold_start"==================================\n\n"bold_end);
                            }

                            system(terminal_clear_string);
                            printf("==================================================================================================\n");
                            printf(bold_start" WATER BILL, ISSUED TO: %s %s | FROM: NATIONAL WATER COMISSION\n"bold_end,user.firstname,user.lastname);
                            printf(" Date of Issue: %02d/%02d/%d          | Due Date: %02d/%02d/%d\n",date.tm_mon +1 , date.tm_mday, date.tm_year + 1900,date.tm_mon +2 , date.tm_mday, date.tm_year + 1900);
                            printf(" Premises ID: %s\n",user.premisesID[premises_pick]);
                            printf(" Previous meter reading: %.2fL  | Current meter reading: %.2fL | Current consumption: %.2fL\n",user.meter_reading[premises_pick],monthly_total,current_consumption);
                            printf("==================================================================================================\n");
                            printf("        Description                             Rate              \t\t Subtotal \t\n");
                            printf("==================================================================================================\n");
                            printf(" Water Charge                                  %.2f               \t\t  %.2f \n",water_rate,water_charge);
                            printf(" Sewerage Charge                               %.2f               \t\t  %.2f \n",sewerage_rate,sewerage_charge);
                            printf(" Meter Service Charge (%dmm)                  %.2f                \t\t  %.2f \n",user.meter_size[premises_pick],service_charge,service_charge);
                            printf(" X factor                                        5%               \t\t  %.2f \n",x_factor);
                            printf(" Price Adjustment Mechanism                     1.21%             \t\t  %.2f \n",PAM);
                            printf(" K factor                                        20%              \t\t  %.2f \n",k_factor);
                            printf("__________________________________________________________________________________________________\n");
                            printf(" Total Current Charges                           --               \t\t  %.2f \n",total_current_charges);
                            if(user.balance_overdue_amt >= 0)
                            {
                            printf(" Balance Carried Over                            --               \t\t  %.2f \n",user.balance_overdue_amt);
                            }
                            else
                            {
                            printf(" Balance Carried Over                            --               \t\t  %.2f \n",user.balance_overdue_amt);
                            }
                            if(early_payment_reduction > 0)
                            {
                            printf(" Early Payment Discount                          5%               \t\t  -%.2f \n",total_current_charges*early_payment_reduction);
                            }
                            printf("==================================================================================================\n");
                            if(total_amount_due<1)
                            {
                                printf(" Total Amount Due                                --               \t\t (%.2f) \n",(total_amount_due*0)+0.0);
                            }
                            else
                            {
                                printf(" Total Amount Due                                --               \t\t (%.2f) \n",total_amount_due);
                            }
                            printf("==================================================================================================\n");


                            //Creating temporary file to make edits
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
                        
                            temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (w+) 
                            //Temporary file now created

                            fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                            int fault_check = 1; // Defauls to false ( no faults found )

                            //Updating customer amount owed and last meter reading
                            if(temp_pointer != NULL && customerdbpointer != NULL) // If both files open successfully, do:
                            {
                                current_line = 1;
                                while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                                {
                                    if(current_line == last_meter_r_location[premises_pick])
                                    {
                                        snprintf(strtemp,max_e_length,lastmeter_r_prefix"%d: %.2f\n",premises_pick+1,monthly_total);
                                        fputs(strtemp,temp_pointer);
                                    }
                                    else if(current_line == balance_overdue_location)
                                    {
                                        snprintf(strtemp,max_e_length,balance_overdue_prefix"%.2f\n",total_amount_due);
                                        fputs(strtemp,temp_pointer);
                                    }
                                    else if(current_line == cycle_location)
                                    {
                                        if(user.balance_overdue_amt<0 && total_amount_due<(user.balance_overdue_amt*-1))
                                        // If user has a credit balance and the total amount due is less than the credit balance
                                        // DONT increment the billing cycle as this bill will be automatically cleared
                                        {
                                            fputs(str,temp_pointer);
                                        }
                                        else
                                        {
                                            snprintf(strtemp,max_e_length,cycle_prefix"%d\n",user.billing_cycle+1);
                                            fputs(strtemp,temp_pointer);
                                        }
                                    }
                                    else
                                    {
                                        fputs(str,temp_pointer);
                                    }
                                    current_line++;
                                }
                            
                                fclose(temp_pointer);
                                fclose(customerdbpointer);
                                clear_stringarray(str);
                                clear_stringarray(strtemp);
                        
                                if(fault_check = remove(customerdatabase) != 0)
                                {
                                    if(debug == 0)
                                    {
                                        perror("\nError msg");
                                        printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                        
                                    }
                                    fault_check = 1;
                                }  
                                else if(fault_check = rename(filename_temp,customerdatabase) != 0)
                                {
                                    if(debug == 0)
                                    {
                                        perror("\nError msg");
                                        printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                    }
                                    fault_check = 1;
                                }
                            }
                            else
                            {
                                printf(file_open_error);
                            }


                            if(fault_check == 0) // If no faults were found
                            {
                                // ADDING billing info to logs for customer access
                                billfilepointer = fopen(billfile,"a+");

                                if(billfilepointer != NULL)
                                {

                                    // If bill hasn't been generated for this premises before, then:
                                    if(duplicate_check(user.premisesID[premises_pick],billfile,premisesid_prefix) == 2)
                                    {
                                        if(debug == 0)
                                        {
                                            printf(bold_start"\nNEW BILL GENERATED\n\n"bold_end);
                                        }
                                        snprintf(str,max_e_length,premisesid_prefix" %s\n",user.premisesID[premises_pick]);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,id_prefix"%s\n",user.customerID);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,date_of_i_prefix"%02d/%02d/%d\n",date.tm_mon +1,date.tm_mday,date.tm_year + 1900);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,due_d_prefix"%02d/%02d/%d\n",date.tm_mon +2,date.tm_mday,date.tm_year + 1900);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,lastmeter_r_prefix" %.2f\n",user.meter_reading[premises_pick]);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,current_meter_r_prefix"%.2f\n",monthly_total);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,water_r_prefix"%.2f\n",water_rate);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,sewerage_r_prefix"%.2f\n",sewerage_rate);
                                        fputs(str,billfilepointer);
                                        
                                        snprintf(str,max_e_length,service_c_prefix"%.2f\n",service_charge);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,early_pr_prefix"%.2f\n",early_payment_reduction);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,balance_tbp_prefix"%.2f\n",total_amount_due);
                                        fputs(str,billfilepointer);

                                        snprintf(str,max_e_length,balance_overdue_prefix"%.2f\n",user.balance_overdue_amt);
                                        fputs(str,billfilepointer);

                                        if(total_amount_due>0)
                                        {
                                            snprintf(str,max_e_length,bill_status_prefix"1\n");
                                            fputs(str,billfilepointer);
                                        }
                                        else
                                        {
                                            snprintf(str,max_e_length,bill_status_prefix"0\n");
                                            fputs(str,billfilepointer);  
                                        }

                                        fputs(data_breakpoint,billfilepointer);
                                        fputs("\n\n",billfilepointer);

                                        printf(bold_start"-User Bill Generated & Stored-"bold_end);
                                    }
                                    
                                    // if bill has been generated for this record before, then:
                                    else if(duplicate_check(user.premisesID[premises_pick],billfile,premisesid_prefix) == 1)
                                    {

                                        //GETTING PREMISES BILL DATA
                                        found_breakpoint = 1; //Setting found breakpoint ";" back to false
                                        found_ID = 1; //Setting found ID back to false
                                        
                                        current_line = 1;
                                        //Locating data related to premises ID
                                        while(fgets(str,max_e_length,billfilepointer) != NULL && found_breakpoint != 0)
                                        {
                                            if(strstr(str,user.premisesID[premises_pick])!=NULL) //If id is found
                                            {
                                                found_ID = 0;
                                                premisesID_location[premises_pick] = current_line;
                                            }
                                            
                                            if(found_ID == 0)
                                            {
                                                if(strstr(str, data_breakpoint)!=NULL)
                                                {
                                                    found_breakpoint = 0;   
                                                    breakpoint_location = current_line;
                                                    break;
                                                }
                                                if(strstr(str,premisesid_prefix) != NULL)
                                                {
                                                    premisesID_location[premises_pick] = current_line;
                                                }
                                                else if(strstr(str,id_prefix) != NULL)
                                                {
                                                    id_location = current_line;
                                                }
                                                else if(strstr(str,date_of_i_prefix) != NULL)
                                                {
                                                    date_oi_location = current_line;
                                                }
                                                else if(strstr(str,due_d_prefix) != NULL)
                                                {
                                                    due_date_location = current_line;
                                                }
                                                else if(strstr(str,lastmeter_r_prefix) != NULL)
                                                {
                                                    last_meter_r_location[premises_pick] = current_line;
                                                }
                                                else if(strstr(str,current_meter_r_prefix) != NULL)
                                                {
                                                    current_meter_r_location = current_line;
                                                }
                                                else if(strstr(str,water_r_prefix) != NULL)
                                                {
                                                    water_rate_location = current_line;
                                                }
                                                else if(strstr(str,sewerage_r_prefix) != NULL)
                                                {
                                                    sewerage_rate_location = current_line;
                                                }
                                                else if(strstr(str,service_c_prefix) != NULL)
                                                {
                                                    service_c_location = current_line;
                                                }
                                                else if(strstr(str,early_pr_prefix) != NULL)
                                                {
                                                    early_pr_location = current_line;
                                                }
                                                else if(strstr(str,balance_overdue_prefix) != NULL)
                                                {
                                                    balance_overdue_location = current_line;
                                                }
                                                else if(strstr(str,balance_tbp_prefix) != NULL)
                                                {
                                                    balance_tbp_location = current_line;
                                                }
                                                else if(strstr(str,bill_status_prefix) != NULL)
                                                {
                                                    bill_status_location = current_line;
                                                }
                                            }
                                            current_line++;
                                        }

                                        //Creating temporary file to make edits
                                        clear_stringarray(filename_temp);
                                        strcpy(filename_temp,billfile); // Copies name of right variable into left
                                    
                                        if(debug == 0)
                                        {
                                            printf(bold_start"=========== DEBUG DATA ===========\n"bold_end);
                                            printf("Premises ID line #: %lu\n",premisesID_location[premises_pick]);
                                            printf("User ID line #: %lu\n",id_location);
                                            printf("Date of Issue line #: %lu\n",date_oi_location);
                                            printf("Due Date line #: %lu\n",due_date_location);
                                            printf("Last Meter Reading line #: %lu\n",last_meter_r_location[premises_pick]);
                                            printf("Current Meter Reading line #: %lu\n",current_meter_r_location);
                                            printf("Water Rate line #: %lu\n",water_rate_location);
                                            printf("Sewerage Rate line #: %lu\n",sewerage_rate_location);
                                            printf("Service Charge line #: %lu\n",service_c_location);
                                            printf("Early Payment Reduction line #: %lu\n",early_pr_location);
                                            printf("Balance Overdue line #: %lu\n",balance_overdue_location);
                                            printf("Bill Status line #: %lu\n",bill_status_location);
                                            printf(bold_start"==================================\n\n"bold_end);
                                        }

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
                                    
                                        temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (w+) 
                                        //Temporary file now created
                                        fseek(billfilepointer,0,SEEK_SET); //Resests pointer to start of file
                                        int fault_check = 1; // Defauls to false ( no faults found )

                                        if(temp_pointer != NULL && billfilepointer != NULL) // If both files open successfully, do:
                                        {
                                            current_line = 1;
                                            while(fgets(str,max_e_length,billfilepointer)!=NULL)
                                            {
                                                if(current_line == id_location)
                                                {
                                                    snprintf(strtemp,max_e_length,id_prefix"%s\n",user.customerID);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == date_oi_location)
                                                {
                                                    snprintf(strtemp,max_e_length,date_of_i_prefix"%02d/%02d/%d\n",date.tm_mon + 1, date.tm_mday, date.tm_year + 1900);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == due_date_location)
                                                {
                                                    snprintf(strtemp,max_e_length,due_d_prefix"%02d/%02d/%d\n",date.tm_mon + 2, date.tm_mday, date.tm_year + 1900);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == last_meter_r_location[premises_pick])
                                                {
                                                    snprintf(strtemp,max_e_length,lastmeter_r_prefix" %.2f\n",user.meter_reading[premises_pick]);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == current_meter_r_location)
                                                {
                                                    snprintf(strtemp,max_e_length,current_meter_r_prefix"%.2f\n",monthly_total);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == water_rate_location)
                                                {
                                                    snprintf(strtemp,max_e_length,water_r_prefix"%.2f\n",water_rate);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == sewerage_rate_location)
                                                {
                                                    snprintf(strtemp,max_e_length,sewerage_r_prefix"%.2f\n",sewerage_rate);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == service_c_location)
                                                {
                                                    snprintf(strtemp,max_e_length,service_c_prefix"%.2f\n",service_charge);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == early_pr_location)
                                                {
                                                    snprintf(strtemp,max_e_length,early_pr_prefix"%.2f\n",early_payment_reduction);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == balance_overdue_location)
                                                {
                                                    snprintf(strtemp,max_e_length,balance_overdue_prefix"%.2f\n",user.balance_overdue_amt);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == balance_tbp_location)
                                                {
                                                    if(total_amount_due<1)
                                                    {
                                                        snprintf(strtemp,max_e_length,balance_tbp_prefix"%.2f\n",(total_amount_due*0)+0.0);
                                                    }
                                                    else
                                                    {
                                                        snprintf(strtemp,max_e_length,balance_tbp_prefix"%.2f\n",total_amount_due);
                                                    }
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else if(current_line == bill_status_location)
                                                {
                                                    if(total_amount_due>0)
                                                    {
                                                        snprintf(strtemp,max_e_length,bill_status_prefix"1");
                                                        fputs(strtemp,temp_pointer);
                                                        fputs("\n",temp_pointer);
                                                    }
                                                    else
                                                    {
                                                        snprintf(strtemp,max_e_length,bill_status_prefix"0");
                                                        fputs(strtemp,temp_pointer);
                                                        fputs("\n",temp_pointer);
                                                    }
                                                }
                                                else
                                                {
                                                    fputs(str,temp_pointer);
                                                }
                                                current_line++;
                                            }
                                        
                                            fclose(temp_pointer);
                                            fclose(billfilepointer);
                                            clear_stringarray(str);
                                            clear_stringarray(strtemp);
                                    
                                            if(fault_check = remove(billfile) != 0)
                                            {
                                                if(debug == 0)
                                                {
                                                    perror("\nError msg");
                                                    printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                                    
                                                }
                                                fault_check = 1;
                                            }  
                                            else if(fault_check = rename(filename_temp,billfile) != 0)
                                            {
                                                if(debug == 0)
                                                {
                                                    perror("\nError msg");
                                                    printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                                }
                                                fault_check = 1;
                                            }
                                            else
                                            {
                                                if(debug == 0)
                                                {
                                                    printf(bold_start"\n-OLD PREMISES RECORD EDITED-\n"bold_end);
                                                }
                                                printf(bold_start"\b-User Bill Generated & Stored-\n"bold_end);
                                            }
                                        }
                                        else
                                        {
                                            printf(file_open_error);
                                        }    
                                    }
                                    else
                                    {
                                        printf(file_open_error);
                                    }
                                    printf(bold_start"Last Meter Reading/s Updated-\n\n"bold_end);
                                }


                            }
                        }
                        else 
                        {
                            printf(text_red_start"\nThis customer dosen't own any premises\n\n"text_color_end);
                        }
                    }
                    else
                    {
                        printf(text_red_start"\nCustomer has 2 unpaid billing cycles - bill cannot be generated\n\n"text_color_end);
                    }
                }
                else
                {
                    printf(text_red_start"\nBill cannot be generated for archived account\n"text_color_end);
                }

                fclose(customerdbpointer);
                fclose(billfilepointer);

            }    
            else
            {
                perror("\nError msg");
            }
            break;
        case '6': // View reports
            customerdbpointer = fopen(customerdatabase, "r"); // attempts to open file in READ mode (r)

            if(customerdbpointer != NULL)
            {
                char customers_to_view;
                char reason[max_e_length];
                customers_to_view = '0';
                printf(bold_start"=========================== VIEW REPORTS =========================== \n\n"bold_end); //Outputting file header
                printf(underline_start"Select View Condition For report\n"underline_end"(P) - Paid Customers\n(O) - Owing Customers\n(A) - Archived Customers\n");
                scanf(" %c", &customers_to_view);
                
                while(customers_to_view != 'A' && customers_to_view != 'a' && customers_to_view != 'P' && customers_to_view != 'p' && customers_to_view != 'O' && customers_to_view != 'o')
                {
                    system(terminal_clear_string); // Clears command line UI
                    printf(bold_start"=========================== VIEW REPORTS =========================== \n\n"bold_end); //Outputting file header
                    printf(bold_start"\n-Invalid Selection-\n"bold_end);
                    printf(underline_start"Select View Condition For report\n"underline_end"(P) - Paid Customers\n(O) - Owing Customers\n(A) - Archived Customers\n"underline_end);
                    scanf(" %c", &customers_to_view);
                }

                if(customers_to_view == 'P' || customers_to_view == 'p')
                {
                    system(terminal_clear_string); //Clears command line 
                    clear_stringarray(str);

                    // Storing field headers into a string to calculate the length of the file headers
                    snprintf(str,max_e_length,"\n     ID         FIRST NAME            LASTNAME              # of PREMISES OWNED     PREMISES #1     PREMISES #2     PREMISES #3     PREMISES #4     PREMISES #5     AMOUNT PAID ($)    \n");
                    int footer_line_width = strlen(str);
                    char header_border[max_e_length];

                    // Creating file header based on length of ^
                    for(i = 0; i<(footer_line_width-strlen(" PAID CUSTOMERS "))/2; i++)
                    {
                        header_border[i] = '=';
                    }
                    header_border[i] = '\0';

                    //Outputting file header
                    printf(bold_start"%s PAID CUSTOMERS %s"bold_end,header_border,header_border); 
                    printf("%s",str);
                    
                    clear_stringarray(str);
                    for(i = 0; i<footer_line_width; i++)
                    {
                        str[i] = '=';
                    }

                    printf("%s\n",str);

                    char fname_spaces[max_e_length];
                    char lname_spaces[max_e_length];

                    customeramt = 0;

                    while(feof(customerdbpointer) == 0)
                    //Loop continues until end of file is reached
                    {
                        // Printing Each customer data
                        if(customeramt>0)
                        {
                            if(user.balance_overdue_amt<=0)
                            {
                                clear_stringarray(fname_spaces);
                                clear_stringarray(lname_spaces);

                                for(int i = 0; i < 21-(strlen(user.firstname)); i++)
                                // 20 is the amount of space that should always be maintained between the first name and the last name
                                {
                                    strcat(fname_spaces," ");
                                }
                                for(int i = 0; i < 30-(strlen(user.lastname)); i++)
                                {
                                    strcat(lname_spaces," ");
                                }

                                if(premisesamt-surrendered_premises_amt<0)
                                {
                                    premisesamt = 0;
                                    surrendered_premises_amt = 0;
                                }
                                
                                // Printing customers that fit criteria
                                char premises_spaces[max_e_length] = "         ";
                                char premises_spaces2[max_e_length] = "                ";
                                
                                printf("   %s        ",user.customerID); // Printing customer ID
                                printf("%s",user.firstname); // Printing customer first name
                                printf("%s%s",fname_spaces,user.lastname); // Printing customer last name
                                printf("%s%d        ",lname_spaces,premisesamt-surrendered_premises_amt); // Printing premises amount
                                for(int i = 0; i<5; i++)
                                {
                                    printf("%s%s",premises_spaces,user.premisesID[i]); // Printing premises ID
                                }
                                
                                printf("%s%.2f\n",premises_spaces,user.balance_overdue_amt); // Printing balance overdue
                                //Printing line between each customer record
                                clear_stringarray(str);
                                for(i = 0; i<footer_line_width; i++)
                                {
                                    str[i] = '-';
                                }
                                printf("%s\n",str);
                            }

                        }
                        

                        //Getting info from customer database
                        found_ID = 1; //Default to false
                        found_breakpoint = 1; //Default to false
                        premisesamt = 0;
                        surrendered_premises_amt = 0;

                        for(i = 0; i<5; i++)
                        {
                            strcpy(user.premisesID[i],"- N/A -");
                        }

                        while(fgets(str,max_e_length,customerdbpointer) != NULL && found_breakpoint != 0)
                        {

                            if(strstr(str,id_prefix)!=NULL)
                            {
                                found_ID = 0;
                                remove_prefix(str,id_prefix);
                                strsanitize(str,0);
                                strcpy(user.customerID,str);
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
                            //Getting balance overdue
                            else if(found_ID == 0 && strstr(str,balance_overdue_prefix) != NULL)
                            {
                                clear_stringarray(strtemp);
                                strcpy(strtemp,str);
                                remove_prefix(strtemp,balance_overdue_prefix);
                                // converts balance overdue from ^ to float
                                user.balance_overdue_amt = atof(strtemp);
                                if(user.balance_overdue_amt<=0)
                                {
                                    customeramt++; //Increment customer count if they have no balance overdue
                                }
                            }
                            //Getting account status
                            else if(found_ID == 0 && strstr(str,status_prefix) != NULL)
                            {
                                strcpy(user.status,str);
                                user_status_location = current_line;
                            }
                            //Getting income class
                            else if(found_ID == 0 && strstr(str,income_class_prefix) != NULL)
                            {
                                clear_stringarray(strtemp);
                                strcpy(strtemp,str);
                                remove_prefix(strtemp,income_class_prefix);
                                user.incomeclass = atoi(strtemp); // converts income class from str to int
                                income_class_location = current_line;
                            }
                            else if(found_ID == 0 && strstr(str,premisesid_prefix) != NULL)
                            {
                                if(strstr(str,surrender_prefix)==NULL) // Only increment premises count if premises being looked at isnt surrendered
                                {
                                    snprintf(strtemp,max_length,premisesid_prefix"%d: ",premisesamt+1);
                                    remove_prefix(str,strtemp);
                                    strsanitize(str,0);
                                    strcpy(user.premisesID[premisesamt],str); // Saves line with premises ID prefix
                                    premisesamt++; 
                                }
                                else
                                {
                                    surrendered_premises_amt++;
                                }
                            }

                        }
                        //Removes prefix such as "UserID: " from string/s
                        remove_prefix(user.firstname,fname_prefix);
                        remove_prefix(user.lastname,lname_prefix);
                        remove_prefix(user.status,status_prefix);

                        //Removes "\n" from string/s
                        strsanitize(user.firstname,0);
                        strsanitize(user.lastname,0);
                        strsanitize(user.status,0);

                        if(debug == 0)
                        {
                            printf("\n=========== DEBUG DATA ===========\n");
                            printf("UserID (from user): %s\n",user.customerID);
                            printf("UserID strlen: %lu\n",strlen(user.customerID));
        
                            printf("User Status: %s\n",user.status);
                            printf("User Status location: %lu\n\n",user_status_location);
        
                            printf("User Income Class: %d\n",user.incomeclass);
                            printf("User Income Class location: %lu\n",income_class_location);
        
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
        
                            printf("Premises amount: %d\n",premisesamt-surrendered_premises_amt);

                            printf("==================================\n");
                        }
                                                
                    }

                    //Printing line between each customer record
                    clear_stringarray(str);
                    for(i = 0; i<footer_line_width; i++)
                    {
                        str[i] = '=';
                    }
                    printf("%s",str);
                    printf("\n");
                    printf(bold_start"PAID CUSTOMER TOTAL #: %d\n"bold_end,customeramt);
                    printf("%s",str);
                    printf("\n");
                }
                else if(customers_to_view == 'O' || customers_to_view == 'o')
                {
                    system(terminal_clear_string); //Clears command line 
                    clear_stringarray(str);

                    // Storing field headers into a string to calculate the length of the file headers
                    snprintf(str,max_e_length,"\n     ID         FIRST NAME            LASTNAME              # of PREMISES OWNED     PREMISES #1     PREMISES #2     PREMISES #3     PREMISES #4     PREMISES #5     AMOUNT OWED ($)     \n");
                    int footer_line_width = strlen(str);
                    char header_border[max_e_length];

                    // Creating file header based on length of ^
                    for(i = 0; i<(footer_line_width-strlen(" OWING CUSTOMERS "))/2; i++)
                    {
                        header_border[i] = '=';
                    }
                    header_border[i] = '\0';

                    //Outputting file header
                    printf(bold_start"%s OWING CUSTOMERS %s"bold_end,header_border,header_border); 
                    printf("%s",str);
                    
                    clear_stringarray(str);
                    for(i = 0; i<footer_line_width; i++)
                    {
                        str[i] = '=';
                    }

                    printf("%s\n",str);

                    char fname_spaces[max_e_length];
                    char lname_spaces[max_e_length];

                    customeramt = 0;

                    while(feof(customerdbpointer) == 0)
                    //Loop continues until end of file is reached
                    {
                        // Printing Each customer data
                        if(customeramt>0)
                        {
                            if(user.balance_overdue_amt>0)
                            {
                                clear_stringarray(fname_spaces);
                                clear_stringarray(lname_spaces);

                                for(int i = 0; i < 21-(strlen(user.firstname)); i++)
                                // 20 is the amount of space that should always be maintained between the first name and the last name
                                {
                                    strcat(fname_spaces," ");
                                }
                                for(int i = 0; i < 30-(strlen(user.lastname)); i++)
                                {
                                    strcat(lname_spaces," ");
                                }

                                if(premisesamt-surrendered_premises_amt<0)
                                {
                                    premisesamt = 0;
                                    surrendered_premises_amt = 0;
                                }
                                // Printing customers that fit criteria
                                char premises_spaces[max_e_length] = "         ";
                                char premises_spaces2[max_e_length] = "                ";
                                printf("   %s        %s%s%s%s%d%s%s%s%s%s%s%s%s%s%s              %.2f\n",user.customerID,user.firstname,fname_spaces,user.lastname,lname_spaces,premisesamt-surrendered_premises_amt,premises_spaces2,user.premisesID[0],premises_spaces,user.premisesID[1],premises_spaces,user.premisesID[2],premises_spaces,user.premisesID[3],premises_spaces,user.premisesID[4],user.balance_overdue_amt);
                                
                                //Printing line between each customer record
                                clear_stringarray(str);
                                for(i = 0; i<footer_line_width; i++)
                                {
                                    str[i] = '-';
                                }
                                printf("%s\n",str);
                            }

                        }
                        

                        //Getting info from customer database
                        found_ID = 1; //Default to false
                        found_breakpoint = 1; //Default to false
                        premisesamt = 0;
                        surrendered_premises_amt = 0;

                        for(i = 0; i<5; i++)
                        {
                            strcpy(user.premisesID[i],"- N/A -");
                        }

                        while(fgets(str,max_e_length,customerdbpointer) != NULL && found_breakpoint != 0)
                        {

                            if(strstr(str,id_prefix)!=NULL)
                            {
                                found_ID = 0;
                                remove_prefix(str,id_prefix);
                                strsanitize(str,0);
                                strcpy(user.customerID,str);
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
                            //Getting balance overdue
                            else if(found_ID == 0 && strstr(str,balance_overdue_prefix) != NULL)
                            {
                                clear_stringarray(strtemp);
                                strcpy(strtemp,str);
                                remove_prefix(strtemp,balance_overdue_prefix);
                                // converts balance overdue from ^ to float
                                user.balance_overdue_amt = atof(strtemp);
                                if(user.balance_overdue_amt>0)
                                {
                                    customeramt++; //Increment customer count if they have no balance overdue
                                }
                            }
                            //Getting account status
                            else if(found_ID == 0 && strstr(str,status_prefix) != NULL)
                            {
                                strcpy(user.status,str);
                                user_status_location = current_line;
                            }
                            //Getting income class
                            else if(found_ID == 0 && strstr(str,income_class_prefix) != NULL)
                            {
                                clear_stringarray(strtemp);
                                strcpy(strtemp,str);
                                remove_prefix(strtemp,income_class_prefix);
                                user.incomeclass = atoi(strtemp); // converts income class from str to int
                                income_class_location = current_line;
                            }
                            else if(found_ID == 0 && strstr(str,premisesid_prefix) != NULL)
                            {
                                if(strstr(str,surrender_prefix)==NULL) // Only increment premises count if premises being looked at isnt surrendered
                                {
                                    snprintf(strtemp,max_length,premisesid_prefix"%d: ",premisesamt+1);
                                    remove_prefix(str,strtemp);
                                    strsanitize(str,0);
                                    strcpy(user.premisesID[premisesamt],str); // Saves line with premises ID prefix
                                    premisesamt++; 
                                }
                                else
                                {
                                    surrendered_premises_amt++;
                                }
                            }

                        }
                        //Removes prefix such as "UserID: " from string/s
                        remove_prefix(user.firstname,fname_prefix);
                        remove_prefix(user.lastname,lname_prefix);
                        remove_prefix(user.status,status_prefix);

                        //Removes "\n" from string/s
                        strsanitize(user.firstname,0);
                        strsanitize(user.lastname,0);
                        strsanitize(user.status,0);

                        if(debug == 0)
                        {
                            printf("\n=========== DEBUG DATA ===========\n");
                            printf("UserID (from user): %s\n",user.customerID);
                            printf("UserID strlen: %lu\n",strlen(user.customerID));
        
                            printf("User Status: %s\n",user.status);
                            printf("User Status location: %lu\n\n",user_status_location);
        
                            printf("User Income Class: %d\n",user.incomeclass);
                            printf("User Income Class location: %lu\n",income_class_location);
        
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
        
                            printf("Premises amount: %d\n",premisesamt-surrendered_premises_amt);

                            printf("==================================\n");
                        }
                                                
                    }

                    //Printing line between each customer record
                    clear_stringarray(str);
                    for(i = 0; i<footer_line_width; i++)
                    {
                        str[i] = '=';
                    }
                    printf("%s",str);
                    printf("\n");
                    printf(bold_start"OWING CUSTOMER TOTAL #: %d\n"bold_end,customeramt);
                    printf("%s",str);
                    printf("\n");
                }
                else if(customers_to_view == 'A' || customers_to_view == 'a')
                {
                    system(terminal_clear_string); //Clears command line 
                    clear_stringarray(str);

                    // Storing field headers into a string to calculate the length of the file headers
                    snprintf(str,max_e_length,"\n     ID     FIRST NAME       LASTNAME       PREMISES #1     PREMISES #2     PREMISES #3     PREMISES #4     PREMISES #5     OUTSTD BAL($)               REASON FOR DELETION             DATE ARCHIVED     \n");
                    int footer_line_width = strlen(str);
                    char header_border[max_e_length];
                    char date_archived[max_length];

                    // Creating file header based on length of ^
                    for(i = 0; i<(footer_line_width-strlen(" ARCHIVED CUSTOMERS "))/2; i++)
                    {
                        header_border[i] = '=';
                    }
                    header_border[i] = '\0';

                    //Outputting file header
                    printf(bold_start"%s ARCHIVED CUSTOMERS %s"bold_end,header_border,header_border); 
                    printf("%s",str);
                    
                    clear_stringarray(str);
                    for(i = 0; i<footer_line_width; i++)
                    {
                        str[i] = '=';
                    }

                    printf("%s\n",str);

                    char fname_spaces[max_e_length];
                    char lname_spaces[max_e_length];

                    customeramt = 0;

                    while(feof(customerdbpointer) == 0)
                    //Loop continues until end of file is reached
                    {
                        // Printing Each customer data
                        if(customeramt>0)
                        {
                            if(strstr(user.status,"ARCHIVED")!=NULL)
                            {
                                clear_stringarray(fname_spaces);
                                clear_stringarray(lname_spaces);

                                for(int i = 0; i < 15-(strlen(user.firstname)); i++)
                                // 20 is the amount of space that should always be maintained between the first name and the last name
                                {
                                    strcat(fname_spaces," ");
                                }
                                for(int i = 0; i < 15-(strlen(user.lastname)); i++)
                                {
                                    strcat(lname_spaces," ");
                                }

                                // Printing customers that fit criteria
                                char premises_spaces[max_length] = "        ";
                                char premises_spaces2[max_length] = "                ";
                                char outstanding_balance_spaces[max_e_length] = "\0";
                                char reason_for_del_spaces[max_e_length] = "\0";

                                //Finding the amount of space needed to keep table "neat"
                                clear_stringarray(strtemp);
                                snprintf(strtemp,max_length,"%d",user.balance_overdue_amt);
                                int space_needed = 16 - strlen(strtemp);
                                int space_needed2 = 40 - strlen(reason);
                                for(int i = 0; i<space_needed; i++)
                                {
                                    strncat(outstanding_balance_spaces," ",max_e_length-max_length);
                                }
                                for(int i = 0; i<space_needed2; i++)
                                {
                                    strncat(reason_for_del_spaces," ",max_e_length-max_length);
                                }
                                printf("   %s    %s%s%s%s%s         %s         %s         %s       %s             %.2f%s%s%s%s\n",user.customerID,user.firstname,fname_spaces,user.lastname,lname_spaces,user.premisesID[0],user.premisesID[1],user.premisesID[2],user.premisesID[3],user.premisesID[4],user.balance_overdue_amt,outstanding_balance_spaces,reason,reason_for_del_spaces,date_archived);
                                
                                //Printing line between each customer record
                                clear_stringarray(str);
                                for(i = 0; i<footer_line_width; i++)
                                {
                                    str[i] = '-';
                                }
                                printf("%s\n",str);
                            }

                        }
                        

                        //Getting info from customer database
                        found_ID = 1; //Default to false
                        found_breakpoint = 1; //Default to false
                        premisesamt = 0;
                        surrendered_premises_amt = 0;

                        for(i = 0; i<5; i++)
                        {
                            strcpy(user.premisesID[i],"- N/A -");
                        }

                        while(fgets(str,max_e_length,customerdbpointer) != NULL && found_breakpoint != 0)
                        {

                            if(strstr(str,id_prefix)!=NULL)
                            {
                                found_ID = 0;
                                remove_prefix(str,id_prefix);
                                strsanitize(str,0);
                                strcpy(user.customerID,str);
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
                            //Getting balance overdue
                            else if(found_ID == 0 && strstr(str,balance_overdue_prefix) != NULL)
                            {
                                clear_stringarray(strtemp);
                                strcpy(strtemp,str);
                                remove_prefix(strtemp,balance_overdue_prefix);
                                // converts balance overdue from ^ to float
                                user.balance_overdue_amt = atof(strtemp);
                            }
                            //Getting account status
                            else if(found_ID == 0 && strstr(str,status_prefix) != NULL)
                            {
                                remove_prefix(str,status_prefix);
                                strcpy(user.status,str);
                                user_status_location = current_line;
                                if(strstr(user.status,"ARCHIVED")!=NULL)
                                {
                                    customeramt++; //Increment customer count if they are archived
                                }
                            }
                            //Getting reason for deletion/archive
                            else if(found_ID == 0 && strstr(str,reason_prefix) != NULL)
                            {
                                remove_prefix(str,reason_prefix);
                                strsanitize(str,0);
                                strcpy(reason,str);
                            }
                            //Getting date of deletion/archive
                            else if(found_ID == 0 && strstr(str,date_archived_prefix) != NULL)
                            {
                                remove_prefix(str,date_archived_prefix);
                                strsanitize(str,0);
                                strcpy(date_archived,str);
                            }
                            //Getting income class
                            else if(found_ID == 0 && strstr(str,income_class_prefix) != NULL)
                            {
                                clear_stringarray(strtemp);
                                strcpy(strtemp,str);
                                remove_prefix(strtemp,income_class_prefix);
                                user.incomeclass = atoi(strtemp); // converts income class from str to int
                                income_class_location = current_line;
                            }
                            else if(found_ID == 0 && strstr(str,premisesid_prefix) != NULL)
                            {
                                if(strstr(str,surrender_prefix)==NULL) // Only increment premises count if premises being looked at isnt surrendered
                                {
                                    snprintf(strtemp,max_length,premisesid_prefix"%d: ",premisesamt+1);
                                    remove_prefix(str,strtemp);
                                    strsanitize(str,0);
                                    strcpy(user.premisesID[premisesamt],str); // Saves line with premises ID prefix
                                    premisesamt++; 
                                }
                                else
                                {
                                    surrendered_premises_amt++;
                                }
                            }

                        }
                        //Removes prefix such as "UserID: " from string/s
                        remove_prefix(user.firstname,fname_prefix);
                        remove_prefix(user.lastname,lname_prefix);
                        remove_prefix(user.status,status_prefix);

                        //Removes "\n" from string/s
                        strsanitize(user.firstname,0);
                        strsanitize(user.lastname,0);
                        strsanitize(user.status,0);

                        if(debug == 0)
                        {
                            printf("\n=========== DEBUG DATA ===========\n");
                            printf("UserID (from user): %s\n",user.customerID);
                            printf("UserID strlen: %lu\n",strlen(user.customerID));
        
                            printf("User Status: %s\n",user.status);
                            printf("User Status location: %lu\n\n",user_status_location);
        
                            printf("User Income Class: %d\n",user.incomeclass);
                            printf("User Income Class location: %lu\n",income_class_location);
        
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
        
                            printf("Premises amount: %d\n",premisesamt-surrendered_premises_amt);

                            printf("==================================\n");
                        }
                                                
                    }

                    //Printing line between each customer record
                    clear_stringarray(str);
                    for(i = 0; i<footer_line_width; i++)
                    {
                        str[i] = '=';
                    }
                    printf("%s",str);
                    printf("\n");
                    printf(bold_start"ARCHIVED CUSTOMER TOTAL #: %d\n"bold_end,customeramt);
                    printf("%s",str);
                    printf("\n");
                }

                fclose(customerdbpointer);
            }
            else
            {
                printf(file_open_error);
            }
            break;
            
        case '7': // View Audit Logs
            char datel[max_length];
            char time[max_length];
            char audit_desc[max_e_length];
            char audit_desc_spaces[max_e_length];
            char id_desc[max_length];
            char id_desc_spaces[max_e_length];
            char user_id[max_length];

            auditpointer = fopen(auditfile, "r"); // attempts to open file in READ mode (r)

            if(auditpointer!=NULL)
            {
                printf(bold_start"========================================= AUDIT LOGS (MM/DD/YY - H/MM) =========================================\n"bold_end); //Outputting file header

                current_line = 1;
                while(fgets(str, max_e_length,auditpointer) != NULL)
                //Explanation: while fgets isnt at the end of the file, do:
                {
                    if(current_line>2) // Skipping file header
                    {
                        fscanf(auditpointer,"%s %*s %s %*s %s %*s %s %s",datel,time,audit_desc,id_desc,user_id);

                        //Finding the amount of space needed to keep table "neat"
                        int space_needed = 50 - strlen(audit_desc);
                        int space_needed2 = 25 - strlen(id_desc);
                        
                        clear_stringarray(audit_desc_spaces);
                        clear_stringarray(id_desc_spaces);

                        for(int i = 0; i<space_needed; i++)
                        {
                            strncat(audit_desc_spaces," ",max_e_length-max_length);
                        }
                        for(int i = 0; i<space_needed2; i++)
                        {
                            strncat(id_desc_spaces," ",max_e_length-max_length);
                        }

                        if((strlen(datel) + strlen(time) + strlen(audit_desc)+ strlen(id_desc)) != 0) // If all fields contain contain data, do:
                        {
                            printf("  %s - %s | %s%s| %s%s%s\n",datel,time,audit_desc,audit_desc_spaces,id_desc,id_desc_spaces,user_id);
                        }
                        else
                        {
                            printf("================================================== NO DATA FOUND ===============================================\n");
                            break;
                        }
                    }
                    current_line++;
                }
                printf(bold_start"================================================================================================================\n"bold_end);
                    
                if(debug == 0)
                {
                    printf(bold_start"=========== DEBUG DATA ===========\n"bold_end);
                    printf("data_breakpoint test: %s\n\n", data_breakpoint);
                    printf(bold_start"==================================\n\n"bold_end);
                }
                fclose(auditpointer);
            }
            else
            {
                printf(file_open_error);
            }
            break;

        case '0': // Close terminal
            close_console(delay_time,print_delay);
            break;
    }

    printf("When you're done enter (X):\n");
    scanf(" %c", &send_back_variable);

    if(send_back_variable == 'X' || send_back_variable == 'x')
    {
        goto jump_admin_actions; // Jumps code back to specified point if logic returns true
    }

}

/************************* CUSTOMER TERMINAL  **********************/

void customer_terminal(char *terminal_clear_string, char* id_of_current_account, int is_user_new_account)
{
    customer user; // Declare struct variabile

    int action;
    int found_ID = 1; //Default to false
    int found_breakpoint = 1; //Default to false
    int premisesamt;
    int surrendered_premises_amt;
    char datestr1[max_length] = {0};
    char datestr2[max_length] = {0};
    char hash_card_number[max_length];
    char hash_cvv[max_length];
    int max_card_length = 6;
    int max_cvv_length = 3;
    int registered_card_amt;
    
    // Variables meant to temporarily store data as its looked at or quickly used then deleted
    char str[max_e_length];
    char strtemp[max_e_length];
    char strtemp2[max_e_length];
    char filename_temp[max_length];
    char temp_card_number[max_length];
    char temp_id[max_length]; 

    char send_back_variable;//Will be used in if function to send user back to select customer actions
    char choice;
    char stall;

    //User data location as unsigned long integers
    long unsigned email_location = 0;
    long unsigned password_location = 0;
    long unsigned fname_location = 0;
    long unsigned lname_location = 0;
    long unsigned balance_overdue_location = 0;
    long unsigned user_status_location = 0;
    long unsigned income_class_location = 0;
    long unsigned premisesID_location[5];
    long unsigned meter_size_location[5] = {0};
    long unsigned last_meter_r_location[5] = {0};
    long unsigned bill_status_location = 0;
    long unsigned bill_balance_tbp_location = 0;
    long unsigned cust_balance_overdue_location = 0;
    long unsigned current_line;
    int user_has_email = 1; //1 for false - 0 for true

    //Variables related to bills
    float early_payment_reduction;
    float water_rate;
    float sewerage_rate;
    float service_charge;
    float monthly_total;
    float current_consumption;
    float water_charge;
    float sewerage_charge;
    float PAM /*Price Adjustment Mechanism*/;
    float x_factor;
    float k_factor;
    float total_current_charges;
    float total_amount_due;
    float bill_amt_to_pay;
    float original_user_balance_od_amt; 
    int bill_status;

    FILE *loginpointer; // Creates file pointer for login database file
    FILE *customerdbpointer; // Creates file pointer for customer database file
    FILE *temp_pointer; // Creates file pointer ( used in making edits to records )
    FILE *billfilepointer; // Creates file pointer for billing log database file
    FILE *cardfilepointer; // Creates file pointer for card database file

    if(debug == 0)
    {
        printf("Program paused to give user chance to see debug\nEnter any character to continue:\n");
        scanf(" %c",&stall);
    }

jump_customer_actions: 

    //Getting user Data
    current_line = 1;
    premisesamt = 0;
    surrendered_premises_amt = 0;       

    loginpointer  = fopen(loginfile,"r"); // Attempts to open file in reading mode (r)
    customerdbpointer = fopen(customerdatabase,"r"); // Attempts to open file in reading mode (r)

    //EMAIL
    if(loginpointer!=NULL)
    {
        while(fgets(str,max_e_length,loginpointer)!=NULL && found_breakpoint != 0)
        {
            if(strstr(str,id_of_current_account)!=NULL) //If id is found
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
            if(found_ID == 0 && strstr(str,password_prefix) != NULL) 
            //IF id has already been found and email prefix "User First Name: " has been found
            //Do:
            {
                password_location = current_line;
            }
            current_line++;
        }
        fclose(loginpointer);
    }
    else
    {
        printf(file_open_error);
    }

    found_breakpoint = 1; //Setting found breakpoint ";" back to false
    found_ID = 1; //Setting found ID back to false
    
    int i = 0;
    
    current_line = 1;
    // GETTING EVERYTHING ELSE IN CUSTOMER DATABASE
    if(customerdbpointer!=NULL)
    {
        while(fgets(str,max_e_length,customerdbpointer) != NULL && found_breakpoint != 0)
        {
            if(strstr(str,id_of_current_account)!=NULL) //If id is found
            {
                found_ID = 0;
            }

            if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
            {
                found_breakpoint = 0;   
                break;
            }

            if(found_ID == 0)
            {
                //Getting first name
                if(strstr(str,fname_prefix) != NULL)
                //IF id has already been found and fname prefix "User First Name: " has been found
                //Do:
                {
                    strcpy(user.firstname,str);
                    fname_location = current_line;
                }
                //Getting last name
                else if(strstr(str,lname_prefix) != NULL)
                {
                    strcpy(user.lastname,str);
                    lname_location = current_line;
                }
                //Getting billing cycle amt
                else if(strstr(str,cycle_prefix) != NULL)
                {
                    remove_prefix(str,cycle_prefix);
                    user.billing_cycle = atoi(str); // converts str to int
                    cycle_location = current_line;
                }
                //Getting balance overdue amt
                else if(strstr(str,balance_overdue_prefix) != NULL)
                {
                    clear_stringarray(strtemp);
                    strcpy(strtemp,str);
                    remove_prefix(strtemp,balance_overdue_prefix);
                    original_user_balance_od_amt = atof(strtemp); // converts income class from str to int
                    cust_balance_overdue_location = current_line;
                }
                //Getting account status
                else if(strstr(str,status_prefix) != NULL)
                {
                    strcpy(user.status,str);
                    user_status_location = current_line;
                }
                //Getting income class
                else if(strstr(str,income_class_prefix) != NULL)
                {
                    clear_stringarray(strtemp);
                    strcpy(strtemp,str);
                    remove_prefix(strtemp,income_class_prefix);
                    user.incomeclass = atoi(strtemp); // converts income class from str to int
                    income_class_location = current_line;
                }


                //Getting premises info
                char final_prefix[max_length];
                char i_as_string[2]; // i + terminating char "\0"  
                if(1)
                {
                    snprintf(i_as_string,12,"%d",i+1); // converts the current value of i to a string and stores it in "i_as_string"
                    // Check for Premises ID
                    if(strstr(str,premisesid_prefix) != NULL)
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

                        if(strstr(str,surrender_prefix)!=NULL)
                        {
                            surrendered_premises_amt++;
                        }

                    }
                    
                    // Check for Meter Size
                    else if (strstr(str, metersize_prefix) != NULL)
                    {
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
            }
            current_line++;
        }
        fclose(customerdbpointer);
    }
    else
    {
        printf(file_open_error);
    }   


    //Removes prefix such as "UserID: " from string/s
    if(strlen(user.email)>0)
    {
        remove_prefix(user.email,email_prefix);
    }
    remove_prefix(user.firstname,fname_prefix);
    remove_prefix(user.lastname,lname_prefix);
    remove_prefix(user.status,status_prefix);

    //Removes "\n" from string/s
    strsanitize(user.email,0);
    strsanitize(user.firstname,0);
    strsanitize(user.lastname,0);
    strsanitize(user.status,0);

    // Getting number of cards registered under user
    cardfilepointer = fopen(paymentcardfile,"r"); // attempts to open file in READ mode (r)

    registered_card_amt = 0;
    current_line = 1;

    if(cardfilepointer!=NULL)
    {
        while(fgets(str,max_e_length,cardfilepointer) != NULL)
        {
            if(current_line>1)
            {
                if(strstr(str,id_of_current_account)!=NULL) //If id is found
                {
                    registered_card_amt++;
                }
            }
            current_line++;
        }
        fclose(cardfilepointer);
    }

    //DEBUG DATA
    if(debug == 0)
    {
        system(terminal_clear_string);
        printf("\n=========== DEBUG DATA ===========\n");
        printf("UserID (from user): %s\n",id_of_current_account);
        printf("UserID strlen: %lu\n",strlen(id_of_current_account));

        printf("User Status: %s\n",user.status);
        printf("User Status location: %lu\n\n",user_status_location);

        printf("User Income Class: %d\n",user.incomeclass);
        printf("User Income Class location: %lu\n",income_class_location);

        if(strlen(user.email)>11) // Email match was found
        {
            printf("\nEmail (from file): %s\n",user.email);
            printf("Email location #: %lu\n",email_location);
            printf("Email strlen: %lu\n",strlen(user.email));
            printf("Password location #: %lu\n",password_location);
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

        //Printing Premises data
        if(premisesamt>0) // If premises were found
        {
            printf(bold_start"\nPremises info (from file):\n"bold_end);
            for(i = 0; i < premisesamt; i++)
            {
                printf(premisesid_prefix"%d: %s\n",i+1, &user.premisesID[i][0]);
                printf("Location line #: %lu\n",premisesID_location[i]);
                printf(metersize_prefix"%d: %d\n",i+1, user.meter_size[i]);
                printf("Location line #: %lu\n",meter_size_location[i]);
                printf(lastmeter_r_prefix"%d: %.2f\n",i+1, user.meter_reading[i]); 
                printf("Location line #: %lu",last_meter_r_location[i]);
                    
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

        printf("\nProgram paused to give user chance to see debug\nEnter any character to continue:\n");
        scanf(" %c",&stall);
    }

    system(terminal_clear_string); // Clears command line UI

    //Outputting header
    printf(bold_start"================== CUSTOMER TERMINAL ==================\n\n"bold_end);
    printf(bold_start"WELCOME! %s %s\n"bold_end,user.firstname,user.lastname);
    
    if(is_user_new_account == 0)//if true
    {
        char update_password;
        printf(bold_start"\nIt seems you have not changed your default password from ************@nzhi"bold_end underline_start"\n\nWould you like to update your password for added security, (Y) Yes or (N) No?:\n"underline_end);
        scanf(" %c",&update_password);
        
        if(update_password == 'Y' || update_password == 'y')
        {
            printf(bold_start"\nEnter new password:\n"bold_end);
            scanf(" %s",user.password);
            
            //Gets password from user
            char temp_passwordtest[max_length];
            clear_stringarray(temp_passwordtest);
            strcpy(temp_passwordtest,id_of_current_account);
            strcat(temp_passwordtest,"@nzhi");
            
            while(strlen(user.password) < 8 || strlen(user.password)>31 || strstr(user.password,temp_passwordtest) != NULL)
            {
                if(strlen(user.password)>31)
                {
                    printf(bold_start"\nPassword should be AT MOST 31 characters long\n"bold_end);
                    printf(underline_start"\nEnter new password \n( %d characters max ):\n"underline_end,password_length);
                    scanf(" %s",user.password);
                    continue;
                }
                else if (strstr(user.password,temp_passwordtest) != NULL)
                {
                    printf(bold_start"\nDefault password cannot be used\n"bold_end);
                    printf(underline_start"\nEnter new password \n( %d characters max ):\n"underline_end,password_length);
                    scanf(" %s",user.password);
                    continue;
                }
                printf(bold_start"\nPassword should be AT LEAST 8 characters long\n"bold_end);
                printf(underline_start"\nEnter new password \n( %d characters max ):\n"underline_end,password_length);
                scanf(" %s",user.password);
            }
            
            clear_stringarray(str);
            strcpy(str,user.password);
            clear_stringarray(user.password);
            hash_djb2(str,user.password);
    
            loginpointer = fopen(loginfile,"r"); // Attempts to open file in read mode (r)
    
            //CHANGING PASSWORD
            //Creating temporary file to make edits
            clear_stringarray(filename_temp);
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
                    if(current_line == password_location)
                    {
                        clear_stringarray(strtemp);
                        snprintf(strtemp,max_e_length,password_prefix"%s\n",user.password);
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
                    audit_editcustomer(9,id_of_current_account);
            
                    system(terminal_clear_string);
                    //Outputting header
                    printf(bold_start"================== CUSTOMER TERMINAL ==================\n\n"bold_end);
                    
                    printf(bold_start"WELCOME! %s %s - password has been updated\n"bold_end,user.firstname,user.lastname);
                    is_user_new_account = 1; // Sets user to not new account
                }
            }
            else
            {
                printf(file_open_error);
            }
        }
        else if(update_password == 'N' || update_password == 'n')
        {
            system(terminal_clear_string);
            printf(bold_start"================== CUSTOMER TERMINAL ==================\n\n"bold_end);
                    
            printf(bold_start"WELCOME! %s %s\n"bold_end,user.firstname,user.lastname);
        }
    }

    //Gets current time (Method by "CodeVault" on youtube)
    time_t t = time(NULL);
    struct tm date = *localtime(&t);

    printf(underline_start"\nEnter choice of action:\n"underline_end);
    if(registered_card_amt == 3)
    {
        printf(text_red_start"(1) - Register New Payment Card (MAX of %d REACHED)\n"text_color_end,max_card_amt);
    }
    else
    {
        printf("(1) - Register New Payment Card (%d slot/s available)\n",max_card_amt-registered_card_amt);
    }
    
    printf("(2) - Pay Bill\n");
    printf("(3) - View Previous Bills\n");
    printf("(4) - Surrender Meter\n");
    printf("(0) - Logout\n");

    scanf(" %d", &action);

    switch(action)
    {
        case 1: // Registering payment card
            // Verification check if user has max amount of registered cards
            if(registered_card_amt >= 3)
            {
                printf(text_red_start"\n-You have reached the maximum amount of cards per account-\n"text_color_end);
                printf("\nWhen you're done enter (X):\n");
                scanf(" %c", &send_back_variable);
            
                if(send_back_variable == 'X' || send_back_variable == 'x')
                {
                    goto jump_customer_actions;
                }                
            } 

            // If registered card for user found, do:
            else
            {
                system(terminal_clear_string); // Clears command line UI

                printf(bold_start"================== CUSTOMER TERMINAL ==================\n\n"bold_end); //Outputting file header

                cardfilepointer = fopen(paymentcardfile,"a+"); // attempts to open file in APPEND & READING mode (a+)

                // GETTING CARD NUMBER
                printf(underline_start"Enter card number ( %d digits ):\n"underline_end,max_card_length);
                scanf(" %s",user.card_number[0]);

                //Validation checks
                while(strlen(user.card_number[0]) != max_card_length || strhchar(user.card_number[0]) != 1)
                {
                    if(strhchar(user.card_number[0]) != 1)
                    // While user input has a character in it, do:
                    {
                        printf(bold_start"\n-Card number should only contain numbers-\n"bold_end);
                        printf(underline_start"\nEnter VALID card number:\n"underline_end);
                        scanf(" %s",user.card_number[0]);
                    }
                    else if(strlen(user.card_number[0]) != max_card_length)
                    {
                        printf(bold_start"\n-Card number should be %d digits long-\n"bold_end,max_card_length);
                        printf(underline_start"\nEnter card number of %d digits:\n"underline_end,max_card_length);
                        scanf(" %s",user.card_number[0]);
                    }
                }

                // After card number is entered, check if it already exists in the database
                // If it does, prompt the user to return to customer main menu to try again  
                clear_stringarray(strtemp);
                hash_djb2(user.card_number[0],strtemp);

                //Validation checks ( if card number is already in use by current account)
                while(fgets(str,max_e_length,cardfilepointer) != NULL)
                {
                    if(current_line>1)
                    {
                        if(strstr(str,id_of_current_account) != NULL && strstr(str,strtemp))
                        //If id of user & card # is found under the same record, do:
                        {
                            printf(bold_start"\n-Card number already used by current account-\n"bold_end);

                            printf("\nWhen you're done enter (X):\n");
                            scanf(" %c", &send_back_variable);
                        
                            if(send_back_variable == 'X' || send_back_variable == 'x')
                            {
                                goto jump_customer_actions;
                            }    
                            break;
                        }
                    }
                    current_line++; 
                }

                // GETTING CVV NUMBER 
                printf(underline_start"\nEnter CVV number ( %d digits ):\n"underline_end,max_cvv_length);
                scanf(" %s",user.cvv[0]);

                //Validation checks
                while(strlen(user.cvv[0]) != max_cvv_length || strhchar(user.cvv[0]) != 1)
                {
                    if(strhchar(user.cvv[0]) != 1)
                    // While user input has a character in it, do:
                    {
                        printf(bold_start"\n-CVV number should only contain numbers-\n"bold_end);
                        printf(underline_start"\nEnter VALID CVV number:\n"underline_end);
                        scanf(" %s",user.cvv[0]);
                    }
                    else if(strlen(user.cvv[0]) != max_cvv_length)
                    {
                        printf(bold_start"\n-CVV number should be %d digits long-\n"bold_end,max_cvv_length);
                        printf(underline_start"\nEnter CVV number of %d digits:\n"underline_end,max_cvv_length);
                        scanf(" %s",user.cvv[0]);
                    }
                }

                // GETTING EXPIRY DATE
                int temp_date = 0;
                char temp_date_str[max_length];
                printf(underline_start"\nEnter card expiry date ( M/YYYY ):\n"underline_end);
                printf("example: 12033 - ( January 2033 ):\n");
                scanf(" %s",temp_date_str);
                
                //Validation checks
                temp_date = atoi(temp_date_str); // Converts string to int
                while(temp_date/10000 > 12 || temp_date/10000 < 1 || temp_date%10000 < date.tm_year + 1900 || strhchar(temp_date_str) != 1)
                // While month is greater than 12 OR less than 1 OR year is less than current year, do:
                {
                    if(temp_date/10000 > 12 || temp_date/10000 < 1)
                    {
                        printf(bold_start"\n-Card expiry month should be between 1 and 12-\n"bold_end);
                        printf(underline_start"\nEnter card expiry date ( M/YYYY ):\n"underline_end);
                        printf("example: 12033 - ( January 2033 ):\n");
                        scanf(" %s",temp_date_str);
                        temp_date = atoi(temp_date_str); // Converts string to int
                    }
                    else if(temp_date%10000 < date.tm_year + 1900)
                    {
                        printf(bold_start"\n-Card expiry year should be greater than current year-\n"bold_end);
                        printf(underline_start"\nEnter card expiry date ( M/YYYY ):\n"underline_end);
                        printf("example: 12033 - ( January 2033 ):\n");
                        scanf(" %s",temp_date_str);
                        temp_date = atoi(temp_date_str); // Converts string to int
                    }
                    else if(strhchar(temp_date_str) != 1)
                    // While user input has a character in it, do:
                    {
                        printf(bold_start"\n-Card expiry date should only contain numbers-\n"bold_end);
                        printf(underline_start"\nEnter VALID card expiry date ( M/YYYY ):\n"underline_end);
                        printf("example: 12033 - ( January 2033 ):\n");
                        scanf(" %s",temp_date_str);
                        temp_date = atoi(temp_date_str); // Converts string to int
                    }
                }

                user.card_exp_date.month[0] = temp_date/10000; // Gets month from user input
                user.card_exp_date.year[0] = temp_date%10000;  // Gets year from user input

                hash_djb2(user.card_number[0],hash_card_number);
                hash_djb2(user.cvv[0],hash_cvv);
                
                if(debug == 0)
                {
                    printf(bold_start"=========== DEBUG DATA ===========\n"bold_end);
                    printf("Card Number: %s\n",user.card_number[0]);
                    printf("Card Number Hash: %s\n",hash_card_number);
                    printf("CVV Number: %s\n",user.cvv[0]);
                    printf("CVV Hash: %s\n",hash_cvv);
                    printf("Card Expiry Month: %d\n",user.card_exp_date.month[0]);
                    printf("Card Expiry Year: %d\n",user.card_exp_date.year[0]);
                    printf(bold_start"==================================\n"bold_end);
                }

                if(cardfilepointer != NULL)
                {
                    fprintf(cardfilepointer, "%s\t\t%s\t\t%s\t\t\t%d\t\t\t\t%d\n", id_of_current_account, hash_card_number, hash_cvv, user.card_exp_date.month[0], user.card_exp_date.year[0]);
                    printf(bold_start"\n-Card Registered-\n"bold_end);
                    fclose(cardfilepointer);
                }
                else
                {
                    printf(file_open_error);
                }
            }
            break;

        case 2: // Pay Bill
            if(premisesamt-surrendered_premises_amt>0)
            {
                // Checking if user has registered payment options
                cardfilepointer = fopen(paymentcardfile,"r"); // attempts to open file in READ mode (r)
                
                int card_found = 1; // Default to false
                current_line = 1;
                while(fgets(str,max_e_length,cardfilepointer) != NULL)
                {
                    if(current_line>1)
                    {
                        if(strstr(str,id_of_current_account)!=NULL) //If id is found
                        {
                            card_found = 0; // Setting card found to true
                        }
                    }
                    current_line++;
                }

                fclose(cardfilepointer);

                if(card_found != 0)
                {
                    printf(text_red_start"\nNo Payment Card Registered On Account\n"text_color_end);
    
                }
                // If user has registered payment options, do:
                else
                {
                    system(terminal_clear_string);
                    printf(bold_start"======================================== BILLING TERMINAL ========================================\n"bold_end); //Outputting file header
                    
                    //Printing Owned Premises
                    printf(bold_start"\nPREMISES OWNED %d\n"bold_end,premisesamt-surrendered_premises_amt);
                    for(i = 0; i < premisesamt; i++)
                    {
                        if(strstr(user.premisesID[i],surrender_prefix)==NULL) // Premises being looked at is not surrendered, do:
                        {
                            printf(premisesid_prefix"%d: %s\n",i+1, &user.premisesID[i][0]);
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
                        else
                        {
                            continue;
                        }                   
                    }

                    int premises_pick;
                    printf(underline_start"\nPick premises # to pay bill:\n"underline_end);
                    scanf(" %d" , &premises_pick);

                    while(premises_pick > premisesamt || premises_pick <= 0 || strstr(user.premisesID[premises_pick-1],surrender_prefix)!=NULL)
                    {
                        printf(bold_start"\n-Customer owns no such premises-\n"bold_end);
                        printf(underline_start"\nPick premises # to view bill:\n"underline_end);
                        scanf(" %d" , &premises_pick);  
                    }

                    premises_pick--; // Decrement so that the number matches actual index values

                    billfilepointer = fopen(billfile,"r");

                    // BILL PROCESSING
                    if(billfilepointer != NULL)
                    {
                        found_breakpoint = 1; //Setting found breakpoint ";" back to false
                        found_ID = 1; //Setting found ID back to false

                        // GETTING BILL INFO
                        current_line = 1;
                        while(fgets(str,max_e_length,billfilepointer) != NULL && found_breakpoint != 0)
                        {
                            if(strstr(str,premisesid_prefix)!= NULL && strstr(str,user.premisesID[premises_pick])!= NULL) //If ID for premises is found
                            {
                                found_ID = 0;
                            }
                    
                            if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
                            {
                                found_breakpoint = 0;   
                                break;
                            }
                    
                            if(found_ID == 0)
                            {
                                if(strstr(str,date_of_i_prefix) != NULL)
                                {
                                    remove_prefix(str,date_of_i_prefix);
                                    strsanitize(str,0);
                                    strcpy(datestr1,str);
                                }
                                else if(strstr(str,due_d_prefix) != NULL)
                                {
                                    remove_prefix(str,due_d_prefix);
                                    strsanitize(str,0);
                                    strcpy(datestr2,str);
                                }
                                else if(strstr(str,lastmeter_r_prefix) != NULL)
                                {
                                    remove_prefix(str,lastmeter_r_prefix" ");
                                    user.meter_reading[premises_pick] = atof(str);
                                }
                                else if(strstr(str,current_meter_r_prefix) != NULL)
                                {
                                    remove_prefix(str,current_meter_r_prefix);
                                    monthly_total = atof(str);
                                }
                                else if(strstr(str,water_r_prefix) != NULL)
                                {
                                    remove_prefix(str,water_r_prefix);
                                    water_rate = atof(str);
                                }
                                else if(strstr(str,sewerage_r_prefix) != NULL)
                                {
                                    remove_prefix(str,sewerage_r_prefix);
                                    sewerage_rate = atof(str);
                                }
                                else if(strstr(str,service_c_prefix) != NULL)
                                {
                                    remove_prefix(str,service_c_prefix);
                                    service_charge = atof(str);
                                }
                                else if(strstr(str,early_pr_prefix) != NULL)
                                {
                                    remove_prefix(str,early_pr_prefix);
                                    early_payment_reduction = atof(str);
                                }
                                else if(strstr(str,balance_overdue_prefix) != NULL)
                                {
                                    remove_prefix(str,balance_overdue_prefix);
                                    user.balance_overdue_amt = atof(str);
                                    balance_overdue_location = current_line;
                                }
                                else if(strstr(str,balance_tbp_prefix) != NULL)
                                {
                                    remove_prefix(str,balance_tbp_prefix);
                                    bill_amt_to_pay = atof(str);
                                    bill_balance_tbp_location = current_line;
                                }
                                else if(strstr(str,bill_status_prefix) != NULL)
                                {
                                    remove_prefix(str,bill_status_prefix);
                                    bill_status = atoi(str);
                                    bill_status_location = current_line;
                                } 
                            }
                            current_line++;
                        }
                        fclose(billfilepointer);

                        // If requested premises has log in billing database that hasnt been paid
                        if(found_ID != 1 && bill_status != 0) 
                        {
                            ///Reassembling bill values
                            current_consumption = monthly_total - user.meter_reading[premises_pick];
                            if(current_consumption<0)
                            {
                                current_consumption = 0;
                            }
                            water_charge = current_consumption * water_rate;
                            sewerage_charge = current_consumption * sewerage_rate;
                            PAM /*Price Adjustment Mechanism*/ = .0121 * (water_charge + sewerage_charge + service_charge);
                            x_factor = .05 * (water_charge + sewerage_charge + service_charge);
                            k_factor = .20 * (water_charge + sewerage_charge + service_charge + PAM);
                            total_current_charges = (water_charge + sewerage_charge + service_charge) - (x_factor + k_factor);
                            total_amount_due = (total_current_charges  - (total_current_charges*early_payment_reduction)) + user.balance_overdue_amt;
                            


                            if(1) // Printing bill, empty if statement for organization
                            {
                                system(terminal_clear_string);
                                printf("==================================================================================================\n");
                                printf(bold_start" WATER BILL, ISSUED TO: %s %s | FROM: NATIONAL WATER COMISSION\n"bold_end,user.firstname,user.lastname);
                                printf(" Date of Issue: %s       | Due Date: %s\n",datestr1,datestr2);
                                printf(" Premises ID: %s\n",user.premisesID[premises_pick]);
                                printf(" Previous meter reading: %.2fL  | Current meter reading: %.2fL | Current consumption: %.2fL\n",user.meter_reading[premises_pick],monthly_total,current_consumption);
                                printf("==================================================================================================\n");
                                printf("        Description                             Rate              \t\t Subtotal \t\n");
                                printf("==================================================================================================\n");
                                printf(" Water Charge                                  %.2f               \t\t  %.2f \n",water_rate,water_charge);
                                printf(" Sewerage Charge                               %.2f               \t\t  %.2f \n",sewerage_rate,sewerage_charge);
                                printf(" Meter Service Charge (%dmm)                   %.2f               \t\t  %.2f \n",user.meter_size[premises_pick],service_charge,service_charge);
                                printf(" X factor                                       5%                \t\t  %.2f \n",x_factor);
                                printf(" Price Adjustment Mechanism                    1.21%              \t\t  %.2f \n",PAM);
                                printf(" K factor                                       20%               \t\t  %.2f \n",k_factor);
                                printf("__________________________________________________________________________________________________\n");
                                printf(" Total Current Charges                           --               \t\t  %.2f \n",total_current_charges);
                                if(user.balance_overdue_amt >= 0)
                                {
                                printf(" Balance Carried Over                            --               \t\t  %.2f \n",user.balance_overdue_amt);
                                }
                                else
                                {
                                printf(" Balance Carried Over                            --               \t\t  %.2f \n",user.balance_overdue_amt-total_amount_due);
                                }
                                if(early_payment_reduction > 0)
                                {
                                printf(" Early Payment Discount                          5%               \t\t  -%.2f \n",total_current_charges*early_payment_reduction);
                                }
                                printf("==================================================================================================\n");
                                printf(" Total Amount Due                                --               \t\t (%.2f) \n",total_amount_due);
                                printf("==================================================================================================\n");
                                printf(" Balance left to be paid                         --               \t\t (%.2f) \n",bill_amt_to_pay);
                                printf("==================================================================================================\n");
                            }

                            if(debug == 0)
                            {
                                printf(bold_start"=========== DEBUG DATA ===========\n"bold_end);
                                printf(bold_start"PREMISES #%d\n"bold_end,premises_pick+1);
                                printf("Previous meter reading: %.2f\n",user.meter_reading[premises_pick]);
                                printf("line location: %lu\n",last_meter_r_location[premises_pick]);
                                printf("Current meter reading: %.2f\n",monthly_total);
                                printf("line location: %lu\n",current_meter_r_location);
                                printf("Current consumption: %.2f\n",current_consumption);
                                printf("Water Charge: %.2f\n",water_charge);
                                printf("Sewerage Charge: %.2f\n",sewerage_charge);
                                printf("Service Charge: %.2f\n",service_charge);
                                printf("line location: %lu\n",service_c_location);
                                printf("PAM (Price Adjustment Mechanism): %.2f\n",PAM);
                                printf("X-Factor: %.2f\n",x_factor);
                                printf("K-Factor: %.2f\n",k_factor);
                                printf("Total Current Charges: %.2f\n",total_current_charges);
                                printf("Balance Overdue @ bill generation: %.2f\n",user.balance_overdue_amt);
                                printf("line location (billing logs): %lu\n",balance_overdue_location);
                                printf("CURRENT Balance Overdue: %.2f\n",original_user_balance_od_amt);
                                printf("line location (customer database): %lu\n",cust_balance_overdue_location);
                                printf("Balance To Be Paid: %.2f\n",bill_amt_to_pay);
                                printf("line location: %lu\n",bill_balance_tbp_location);
                                printf("Total Amount Due: %.2f\n",total_amount_due);
                                if(early_payment_reduction == 0)
                                {
                                    printf("Early Payment Reduction: NOT eligible\n");
                                }
                                else
                                {
                                    printf("Early Payment Reduction: Eligible\n");
                                    
                                }
                                printf(bold_start"==================================\n"bold_end);
                            }

                            i = 0;
                            if(bill_status == 0) // If bill has been paid
                            {
                                printf(bold_start"\nTHIS BILL HAS BEEN PAID\n"bold_end);
                            }
                            else // bill has NOT been paid
                            {
                                choice = '3';
                                while(choice != '1' && choice != '2' && choice != '0')
                                {
                                    if(i>0)
                                    {
                                        printf(bold_start"\n-Invalid Option-\n"bold_end);
                                    }
                                    printf(bold_start"\nOptions:\n"bold_end);
                                    printf("(1) - Pay in Full\n(2) - Pay in Partials\n(0) - Return to Menu\n");
                                    scanf(" %c", &choice);
                                    i++;
                                }

                                if(choice == '0')
                                {
                                    goto jump_customer_actions;
                                }
                                else if(choice == '1') // Full Payment
                                {
                                    customerdbpointer = fopen(customerdatabase,"r"); // Attempts to open file in read mode (r)

                                    //Creating temporary file to make edits
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
                                
                                    temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                                    //Temporary file now created
                                
                                    fseek(customerdbpointer,0,SEEK_SET); //Resests pointer to start of file
                            
                                    // Updating balance overdue to show account is fully paid
                                    if(temp_pointer != NULL && customerdbpointer != NULL) // If both files open successfully, do:
                                    {
                                        current_line = 1;
                                        while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                                        {
                                            if(current_line == cust_balance_overdue_location)
                                            {
                                                clear_stringarray(strtemp);
                                                if(debug == 0)
                                                {
                                                    printf("\nCUSTOMER BALANCE OVERDUE, EDITED\n");
                                                }
                                                snprintf(strtemp,max_e_length,balance_overdue_prefix"%.2f\n",(original_user_balance_od_amt-total_amount_due)+0.0);
                                                fputs(strtemp,temp_pointer);
                                            }
                                            else if(current_line == cycle_location)
                                            {
                                                if(original_user_balance_od_amt == total_amount_due)
                                                {
                                                    snprintf(strtemp,max_e_length,cycle_prefix"%d\n",(user.billing_cycle-user.billing_cycle)+0.0);
                                                    fputs(strtemp,temp_pointer); 
                                                } 
                                                else if(user.billing_cycle>0)
                                                {
                                                    snprintf(strtemp,max_e_length,cycle_prefix"%d\n",user.billing_cycle-1);
                                                    fputs(strtemp,temp_pointer); 
                                                }
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
                                
                                        // Replacing original file with copy
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

                                        // If balance was successfully changed to paid, then update bill status & amount to 0 (FULLY PAID)
                                        else
                                        {
                                            billfilepointer = fopen(billfile,"r"); // Attempts to open file in read mode (r)

                                            //CHANGING PASSWORD
                                            //Creating temporary file to make edits
                                            clear_stringarray(filename_temp);
                                            strcpy(filename_temp,billfile); // Copies name of right variable into left
                                        
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
                                        
                                            fseek(billfilepointer,0,SEEK_SET); //Resests pointer to start of file
                                    
                                            if(temp_pointer != NULL && billfilepointer != NULL) // If both files open successfully, do:
                                            {
                                                current_line = 1;
                                                while(fgets(str,max_e_length,billfilepointer)!=NULL)
                                                {
                                                    if(current_line == bill_balance_tbp_location)
                                                    {
                                                        if(debug == 0)
                                                        {
                                                            printf("\nBILL BALANCE TO BE PAID, EDITED to 0\n");
                                                        }
                                                        snprintf(strtemp,max_e_length,balance_tbp_prefix"0\n");
                                                        fputs(strtemp,temp_pointer);
                                                    }
                                                    else if(current_line == bill_status_location)
                                                    {
                                                        if(debug == 0)
                                                        {
                                                            printf("\nBILL STATUS, EDITED to 0\n");
                                                        }
                                                        snprintf(strtemp,max_e_length,bill_status_prefix"0\n");
                                                        fputs(strtemp,temp_pointer);
                                                    }
                                                    else
                                                    {
                                                        fputs(str,temp_pointer);
                                                    }
                                                    current_line++;
                                                }
                                            
                                                fclose(billfilepointer);
                                                fclose(temp_pointer);
                                                clear_stringarray(str);
                                                clear_stringarray(strtemp);
                                        
                                                if(remove(billfile) != 0)
                                                {
                                                    if(debug == 0)
                                                    {
                                                        perror("\nError msg");
                                                        printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                                    }
                                                }  
                                                else if(rename(filename_temp,billfile) != 0)
                                                {
                                                    if(debug == 0)
                                                    {
                                                        perror("\nError msg");
                                                        printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                                    }
                                                }

                                                //Upon successfull execution of all the previous steps, do:
                                                // Printing Receipt
                                                else
                                                {
                                                    struct tm date = *localtime(&t); // getting current time & date  

                                                    setall_uppercase(user.firstname);
                                                    setall_uppercase(user.lastname);
                                                    float service_fee = 60.00;
                                                    srand(time(NULL)); //Sets random function's seed to current time
                                                    
                                                    system(terminal_clear_string);
                                                    printf(bold_start"====================== RECEIPT =======================\n"bold_end);
                                                    printf(" Date Issued      : %s\n", datestr1);
                                                    printf(" Date Due         : %s\n", datestr2);
                                                    printf(" Customer#        : %s\n",id_of_current_account);
                                                    printf(" Premises ID#     : %s\n",user.premisesID[premises_pick]);
                                                    printf(" Receipt#         : %d\n",(100000 + rand() % 999999 - 100000 + 1)*(100000 + rand() % 999999 - 100000 + 1));
                                                    printf(" Type             : Utility Bill ( Water )\n");
                                                    printf(" Meter Reading    : %.2fL\n",monthly_total);
                                                    printf(" Old Meter Reading: %.2fL\n",user.meter_reading[premises_pick]);
                                                    printf(" Customer Name    : %s  %s\n\n",user.firstname,user.lastname);
                                                    
                                                    printf(bold_start" Description\t\t\t\tAmount $\n"bold_end);
                                                    printf(" Current Charges  :\t\t\t%.2f\n",bill_amt_to_pay);
                                                    
                                                    if(original_user_balance_od_amt<1) // If user account has credit 
                                                    {
                                                        printf(" Balance Overdue  :\t\t\t%.2f\n",original_user_balance_od_amt*0);
                                                    }
                                                    else
                                                    {
                                                        printf(" Balance Overdue  :\t\t\t%.2f\n",original_user_balance_od_amt-bill_amt_to_pay);
                                                    }
                                                    
                                                    if(original_user_balance_od_amt<1)
                                                    {
                                                        printf(" Balance B/F      :\t\t\t%.2f\n\n",original_user_balance_od_amt-bill_amt_to_pay);
                                                    }
                                                    else
                                                    {
                                                        printf(" Balance B/F      :\t\t\t%.2f\n\n",original_user_balance_od_amt*0);
                                                    }
                                                    printf(" Service fee      :\t\t\t%.2f\n",service_fee);
                                                    printf(" Recipt Total     :\t\t\t%.2f\n\n",bill_amt_to_pay+service_fee);

                                                    printf(bold_start"~~~~~~~~~~~~~ Thank you for your payment ~~~~~~~~~~~~~\n"bold_end);
                                                    printf(bold_start"======================================================\n"bold_end);
                                                    
                                                    //Undoing setall_uppercase (just incase)
                                                    setall_lowercase(user.firstname);
                                                    setall_lowercase(user.lastname);
                                                }

                                            }
                                        }
                                    }
                                }
                                else if(choice == '2') // Partial Payment
                                {
                                    float payment_amt = 0;
                                    printf(underline_start"\nHow much would you like to pay ( Minimum of 1 JMD ): \n"underline_end);
                                    scanf("%f", &payment_amt);

                                    while(payment_amt == total_amount_due)
                                    {
                                        printf(bold_start"\n-Cannot cover full costs while in partial payments!-\nWe recommend switching to full payment (1) for such transactions\n"bold_end);
                                        printf(underline_start"\nHow much would you like to pay ( Minimum of 1 JMD ): \n"underline_end);
                                        scanf("%f", &payment_amt);
                                    }
                                    while(payment_amt<1.00)
                                    {
                                        printf(bold_start"\n-INVALID PAYMENT AMOUNT ( Minimum of 1 JMD )-\n"bold_end);
                                        printf(underline_start"\nHow much would you like to pay: \n"underline_end);
                                        scanf("%f", &payment_amt);
                                    }


                                    customerdbpointer = fopen(customerdatabase,"r"); // Attempts to open file in read mode (r)

                                    //Creating temporary file to make edits
                                    clear_stringarray(filename_temp);
                                    strcpy(filename_temp,customerdatabase); // Copies name of right variable into left
                                
                                    // Removes .txt from end of file
                                    int a = 0;
                                    for(int b = 1; b<strlen(filename_temp); b++)
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
                            
                                    if(temp_pointer != NULL && customerdbpointer != NULL) // If both files open successfully, do:
                                    {
                                        current_line = 1;
                                        while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                                        {
                                            if(current_line == cust_balance_overdue_location)
                                            {
                                                clear_stringarray(strtemp);
                                                snprintf(strtemp,max_e_length,balance_overdue_prefix"%.2f\n",original_user_balance_od_amt-payment_amt);
                                                fputs(strtemp,temp_pointer);
                                            }
                                            else if(current_line == cycle_location)
                                            {
                                                if(original_user_balance_od_amt <= payment_amt)
                                                {
                                                    snprintf(strtemp,max_e_length,cycle_prefix"%d\n",(user.billing_cycle-user.billing_cycle)+0.0);
                                                    fputs(strtemp,temp_pointer);
                                                }
                                                else
                                                {
                                                    fputs(str,temp_pointer);
                                                }
                                            }
                                            else if(current_line == cycle_location && payment_amt>=total_amount_due)
                                            {
                                                if(user.billing_cycle>0)
                                                {
                                                    snprintf(strtemp,max_e_length,cycle_prefix"%d\n",user.billing_cycle-1);
                                                    fputs(strtemp,temp_pointer); 
                                                }
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
                                            billfilepointer = fopen(billfile,"r"); // Attempts to open file in read mode (r)

                                            //CHANGING PASSWORD
                                            //Creating temporary file to make edits
                                            clear_stringarray(filename_temp);
                                            strcpy(filename_temp,billfile); // Copies name of right variable into left
                                        
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
                                        
                                            fseek(billfilepointer,0,SEEK_SET); //Resests pointer to start of file
                                    
                                            if(temp_pointer != NULL && billfilepointer != NULL) // If both files open successfully, do:
                                            {
                                                current_line = 1;
                                                while(fgets(str,max_e_length,billfilepointer)!=NULL)
                                                {
                                                    if(current_line == bill_status_location && bill_amt_to_pay-payment_amt <= 0)
                                                    // if the current line matches with bill status AND the bill amount being paid is
                                                    // MORE than stipulated on the bill, the bill satus is set to paid on billing_logs
                                                    {
                                                        clear_stringarray(strtemp);
                                                        snprintf(strtemp,max_e_length,bill_status_prefix"0\n");
                                                        fputs(strtemp,temp_pointer);
                                                    }
                                                    else if(current_line == bill_balance_tbp_location)
                                                    {
                                                        float new_value_temp = bill_amt_to_pay-payment_amt;
                                                        if(new_value_temp < 0)
                                                        {
                                                            new_value_temp = 0;
                                                        }
                                                        snprintf(strtemp,max_e_length,balance_tbp_prefix"%.2f\n",new_value_temp);
                                                        fputs(strtemp,temp_pointer);
                                                    }
                                                    else
                                                    {
                                                        fputs(str,temp_pointer);
                                                    }
                                                    current_line++;
                                                }
                                            
                                                fclose(billfilepointer);
                                                fclose(temp_pointer);
                                                clear_stringarray(str);
                                                clear_stringarray(strtemp);
                                        
                                                if(remove(billfile) != 0)
                                                {
                                                    if(debug == 0)
                                                    {
                                                        perror("\nError msg");
                                                        printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                                                    }
                                                }  
                                                else if(rename(filename_temp,billfile) != 0)
                                                {
                                                    if(debug == 0)
                                                    {
                                                        perror("\nError msg");
                                                        printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                                                    }
                                                }
                                                
                                                // Printing Receipt
                                                else
                                                {
                                                    struct tm date = *localtime(&t); // getting current time & date  

                                                    setall_uppercase(user.firstname);
                                                    setall_uppercase(user.lastname);
                                                    float service_fee = 60.00;
                                                    srand(time(NULL)); //Sets random function's seed to current time

                                                    system(terminal_clear_string);
                                                    printf(bold_start"====================== RECEIPT =======================\n"bold_end);
                                                    printf(" Date Issued      : %s\n", datestr1);
                                                    printf(" Date Due         : %s\n", datestr2);
                                                    printf(" Customer#        : %s\n",id_of_current_account);
                                                    printf(" Premises ID#     : %s\n",user.premisesID[premises_pick]);
                                                    printf(" Receipt#         : %d\n",(100000 + rand() % 999999 - 100000 + 1)*(100000 + rand() % 999999 - 100000 + 1));
                                                    printf(" Type             : Utility Bill ( Water )\n");
                                                    printf(" Meter Reading    : %.2fL\n",monthly_total);
                                                    printf(" Old Meter Reading: %.2fL\n",user.meter_reading[premises_pick]);
                                                    printf(" Customer Name    : %s  %s\n\n",user.firstname,user.lastname);
                                                    
                                                    printf(bold_start" Description\t\t\t\tAmount $\n"bold_end);
                                                    printf(" Current Charges  :\t\t\t%.2f\n",payment_amt);
                                                    
                                                    if(original_user_balance_od_amt<1) // If user account has credit 
                                                    {
                                                        printf(" Balance Overdue  :\t\t\t%.2f\n",original_user_balance_od_amt*0);
                                                    }
                                                    else
                                                    {
                                                        printf(" Balance Overdue  :\t\t\t%.2f\n",original_user_balance_od_amt-payment_amt);
                                                    }
                                                    
                                                    if(original_user_balance_od_amt<1)
                                                    {
                                                        printf(" Balance B/F      :\t\t\t%.2f\n\n",original_user_balance_od_amt-payment_amt);
                                                    }
                                                    else
                                                    {
                                                        printf(" Balance B/F      :\t\t\t%.2f\n\n",original_user_balance_od_amt*0);
                                                    }
                                                    printf(" Service fee      :\t\t\t%.2f\n\n",service_fee);
                                                    printf(" Recipt Total     :\t\t\t%.2f\n",payment_amt+service_fee);

                                                    printf(bold_start"~~~~~~~~~~~~~ Thank you for your payment ~~~~~~~~~~~~~\n"bold_end);
                                                    printf(bold_start"======================================================\n"bold_end);
                                                }

                                            }
                                        }
                                    }
                                }
                            }

                        }
                        else
                        {
                            printf(text_red_start"\nNo bill generated for premises\n"text_color_end);
                        }
                    }
                    else
                    {
                        printf(file_open_error);
                    }
                }
            }
            else
            {
                printf(text_red_start"\nNo Premises Owned by this account\n"text_color_end);
            }
            break;

        case 3: // View bills ( most recent )
            if(premisesamt-surrendered_premises_amt>0)
            {
                system(terminal_clear_string);
                printf(bold_start"======================================== BILLING TERMINAL ========================================\n"bold_end); //Outputting file header
                
                //Printing Owned Premises
                printf(bold_start"\nPREMISES OWNED %d\n"bold_end,premisesamt-surrendered_premises_amt);
                for(i = 0; i < premisesamt; i++)
                {
                    if(strstr(user.premisesID[i],surrender_prefix)==NULL)
                    {
                        printf(premisesid_prefix"%d: %s\n",i+1, &user.premisesID[i][0]);
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
                    else
                    {
                        continue;
                    }              
                }

                int premises_pick;
                printf(underline_start"\nPick premises # to view bill:\n"underline_end);
                scanf(" %d" , &premises_pick);

                while(premises_pick > premisesamt || premises_pick <= 0 || strstr(user.premisesID[premises_pick-1],surrender_prefix)!=NULL)
                {
                    printf(bold_start"\n-Customer owns no such premises-\n"bold_end);
                    printf(underline_start"\nPick premises # to view bill:\n"underline_end);
                    scanf(" %d" , &premises_pick);  
                }

                premises_pick--; // Decrement so that the number matches actual index values

                billfilepointer = fopen(billfile,"r");

                // GETTING BILL INFO
                if(billfilepointer != NULL)
                {
                    found_breakpoint = 1; //Setting found breakpoint ";" back to false
                    found_ID = 1; //Setting found ID back to false

                    current_line = 1;
                    while(fgets(str,max_e_length,billfilepointer) != NULL && found_breakpoint != 0)
                    {
                        if(strstr(str,premisesid_prefix)!= NULL && strstr(str,user.premisesID[premises_pick])!= NULL) //If ID for premises is found
                        {
                            found_ID = 0;
                        }
                
                        if(strstr(str, data_breakpoint)!=NULL && found_ID == 0)
                        {
                            found_breakpoint = 0;   
                            break;
                        }
                
                        if(found_ID == 0)
                        {
                            if(strstr(str,date_of_i_prefix) != NULL)
                            {
                                remove_prefix(str,date_of_i_prefix);
                                strsanitize(str,0);
                                strcpy(datestr1,str);
                            }
                            else if(strstr(str,due_d_prefix) != NULL)
                            {
                                remove_prefix(str,due_d_prefix);
                                strsanitize(str,0);
                                strcpy(datestr2,str);
                            }
                            else if(strstr(str,lastmeter_r_prefix) != NULL)
                            {
                                remove_prefix(str,lastmeter_r_prefix" ");
                                user.meter_reading[premises_pick] = atof(str);
                            }
                            else if(strstr(str,current_meter_r_prefix) != NULL)
                            {
                                remove_prefix(str,current_meter_r_prefix);
                                monthly_total = atof(str);
                            }
                            else if(strstr(str,water_r_prefix) != NULL)
                            {
                                remove_prefix(str,water_r_prefix);
                                water_rate = atof(str);
                            }
                            else if(strstr(str,sewerage_r_prefix) != NULL)
                            {
                                remove_prefix(str,sewerage_r_prefix);
                                sewerage_rate = atof(str);
                            }
                            else if(strstr(str,service_c_prefix) != NULL)
                            {
                                remove_prefix(str,service_c_prefix);
                                service_charge = atof(str);
                            }
                            else if(strstr(str,early_pr_prefix) != NULL)
                            {
                                remove_prefix(str,early_pr_prefix);
                                early_payment_reduction = atof(str);
                            }
                            else if(strstr(str,balance_overdue_prefix) != NULL)
                            {
                                remove_prefix(str,balance_overdue_prefix);
                                user.balance_overdue_amt = atof(str);
                                balance_overdue_location = current_line;
                            }
                            else if(strstr(str,balance_tbp_prefix) != NULL)
                            {
                                remove_prefix(str,balance_tbp_prefix);
                                bill_amt_to_pay = atof(str);
                                bill_balance_tbp_location = current_line;
                            }
                            else if(strstr(str,bill_status_prefix) != NULL)
                            {
                                remove_prefix(str,bill_status_prefix);
                                bill_status = atoi(str);
                                bill_status_location = current_line;
                            } 
                        }
                        current_line++;
                    }
                    fclose(billfilepointer);

                    if(found_ID != 1) // If requested premises has log in billing database
                    {
                        ///Reassembling bill values
                        current_consumption = monthly_total - user.meter_reading[premises_pick];
                        if(current_consumption<0)
                        {
                            current_consumption = 0;
                        }
                        water_charge = current_consumption * water_rate;
                        sewerage_charge = current_consumption * sewerage_rate;
                        PAM /*Price Adjustment Mechanism*/ = .0121 * (water_charge + sewerage_charge + service_charge);
                        x_factor = .05 * (water_charge + sewerage_charge + service_charge);
                        k_factor = .20 * (water_charge + sewerage_charge + service_charge + PAM);
                        total_current_charges = (water_charge + sewerage_charge + service_charge) - (x_factor + k_factor);
                        total_amount_due = (total_current_charges  - (total_current_charges*early_payment_reduction)) + user.balance_overdue_amt;
                        
                        if(1) // Printing bill empty if statement for organization
                        {
                            system(terminal_clear_string);
                            printf("==================================================================================================\n");
                            printf(bold_start" WATER BILL, ISSUED TO: %s %s | FROM: NATIONAL WATER COMISSION\n"bold_end,user.firstname,user.lastname);
                            printf(" Date of Issue: %s       | Due Date: %s\n",datestr1,datestr2);
                            printf(" Premises ID: %s\n",user.premisesID[premises_pick]);
                            printf(" Previous meter reading: %.2fL  | Current meter reading: %.2fL | Current consumption: %.2fL\n",user.meter_reading[premises_pick],monthly_total,current_consumption);
                            printf("==================================================================================================\n");
                            printf("        Description                             Rate              \t\t Subtotal \t\n");
                            printf("==================================================================================================\n");
                            printf(" Water Charge                                  %.2f               \t\t  %.2f \n",water_rate,water_charge);
                            printf(" Sewerage Charge                               %.2f               \t\t  %.2f \n",sewerage_rate,sewerage_charge);
                            printf(" Meter Service Charge (%dmm)                   %.2f               \t\t  %.2f \n",user.meter_size[premises_pick],service_charge,service_charge);
                            printf(" X factor                                       5%                \t\t  %.2f \n",x_factor);
                            printf(" Price Adjustment Mechanism                    1.21%              \t\t  %.2f \n",PAM);
                            printf(" K factor                                       20%               \t\t  %.2f \n",k_factor);
                            printf("__________________________________________________________________________________________________\n");
                            printf(" Total Current Charges                           --               \t\t  %.2f \n",total_current_charges);
                            printf(" Balance Carried Over                            --               \t\t  %.2f \n",user.balance_overdue_amt);
                            if(early_payment_reduction > 0)
                            {
                            printf(" Early Payment Discount                          5%               \t\t  -%.2f \n",total_current_charges*early_payment_reduction);
                            }
                            printf("==================================================================================================\n");
                            printf(" Total Amount Due                                --               \t\t (%.2f) \n",total_amount_due);
                            printf("==================================================================================================\n");
                            printf(" Balance left to be paid                         --               \t\t (%.2f) \n",bill_amt_to_pay);
                            printf("==================================================================================================\n");
                        }

                        if(debug == 0)
                        {
                            printf(bold_start"=========== DEBUG DATA ===========\n"bold_end);
                            printf(bold_start"PREMISES #%d\n"bold_end,premises_pick+1);
                            printf("Previous meter reading: %.2f\n",user.meter_reading[premises_pick]);
                            printf("line location: %lu\n",last_meter_r_location[premises_pick]);
                            printf("Current meter reading: %.2f\n",monthly_total);
                            printf("line location: %lu\n",current_meter_r_location);
                            printf("Current consumption: %.2f\n",current_consumption);
                            printf("Water Charge: %.2f\n",water_charge);
                            printf("Sewerage Charge: %.2f\n",sewerage_charge);
                            printf("Service Charge: %.2f\n",service_charge);
                            printf("line location: %lu\n",service_c_location);
                            printf("PAM (Price Adjustment Mechanism): %.2f\n",PAM);
                            printf("X-Factor: %.2f\n",x_factor);
                            printf("K-Factor: %.2f\n",k_factor);
                            printf("Total Current Charges: %.2f\n",total_current_charges);
                            printf("Balance Overdue: %.2f\n",user.balance_overdue_amt);
                            printf("line location (billing logs): %lu\n",balance_overdue_location);
                            printf("line location (customer database): %lu\n",cust_balance_overdue_location);
                            printf("Balance To Be Paid: %.2f\n",bill_amt_to_pay);
                            printf("line location: %lu\n",bill_balance_tbp_location);
                            printf("Total Amount Due: %.2f\n",total_amount_due);
                            if(early_payment_reduction == 0)
                            {
                                printf("Early Payment Reduction: NOT eligible\n");
                            }
                            else
                            {
                                printf("Early Payment Reduction: Eligible\n");
                                
                            }
                            printf(bold_start"==================================\n"bold_end);
                        }

                        i = 0;
                        if(bill_status == 0) // If bill has been paid
                        {
                            printf(bold_start"\nTHIS BILL HAS BEEN PAID\n"bold_end);

                            printf("\nWhen you're done enter (X):\n");
                            scanf(" %c", &send_back_variable);
                        
                            if(send_back_variable == 'X' || send_back_variable == 'x')
                            {
                                goto jump_customer_actions;
                            }

                        }
                    }
                    else
                    {
                        printf(text_red_start"\nNo bill generated for premises\n"text_color_end);
                    }
                }
                else
                {
                    printf(file_open_error);
                }
            }
            else
            {
                printf(text_red_start"\nNo Premises Owned by this account\n"text_color_end);
            }
            break;

        case 4: // Surrendering Meter
            if(premisesamt-surrendered_premises_amt>0)
            {
                //Printing Owned Premises
                system(terminal_clear_string);
                printf(bold_start"======================================== METER SURRENDER ========================================\n\n"bold_end); //Outputting file header
                printf(bold_start"PREMISES OWNED %d\n"bold_end,premisesamt-surrendered_premises_amt);
                for(i = 0; i < premisesamt; i++)
                {
                    if(strstr(user.premisesID[i],surrender_prefix)==NULL)
                    {
                        printf(premisesid_prefix"%d: %s\n",i+1, &user.premisesID[i][0]);
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
                    else
                    {
                        continue;
                    }   
                }

                int premises_pick;
                printf(underline_start"\nPick meter # to surrender:\n"underline_end);
                scanf(" %d" , &premises_pick);

                // Input validation
                while(premises_pick > premisesamt || premises_pick <= 0 || strstr(user.premisesID[premises_pick-1],surrender_prefix)!=NULL)
                {
                    printf(bold_start"\n-Customer owns no such premises-\n"bold_end);
                    printf(underline_start"\nPick premises # to view bill:\n"underline_end);
                    scanf(" %d" , &premises_pick);  
                }

                //Getting confirmation that users wants to surrender meter
                printf(bold_start"\nAre you sure you want to surrender this meter? (Y/N)"bold_end text_red_start"\nTHIS CANNOT BE UNDONE\n"text_color_end);
                char confirmation;
                scanf(" %c", &confirmation);

                while(confirmation != 'Y' && confirmation != 'y' && confirmation != 'N' && confirmation != 'n')
                {
                    printf(bold_start"\n-INVALID INPUT-\n"bold_end);
                    printf(bold_start"\nAre you sure you want to surrender this meter? (Y/N)\nTHIS CAN NOT BE UNDONE AND YOU'LL RETURN TO ACTION MENU UPON SELECTION OF (N)\n"bold_end);
                    scanf(" %c", &confirmation);
                }

                if(confirmation == 'N' || confirmation == 'n')
                {
                    goto jump_customer_actions; // Returns to action menu
                }
                else if(confirmation == 'Y' || confirmation == 'y')
                {
                    premises_pick--; // Decrement so that the number matches actual index values

                    //Creating temporary file to make edits
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
                
                    temp_pointer = fopen(filename_temp,"w+"); // attempts to create/overwrite file in Write & Reading mode (W+) 
                    customerdbpointer = fopen(customerdatabase,"r"); // Attempts to open file in read mode (r)

                    // EDITING DATA ( METER SIZE & STATUS)
                    if(customerdbpointer != NULL && temp_pointer != NULL) // If both files open successfully
                    {
                        current_line = 1;
                        while(fgets(str,max_e_length,customerdbpointer)!=NULL)
                        // While, we're not at the end of file, do:
                        {
                            if(current_line == premisesID_location[premises_pick])
                            {
                                snprintf(strtemp,max_e_length,premisesid_prefix"%d: %s-SR\n",premises_pick+1,user.premisesID[premises_pick]);
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

                        if(remove(customerdatabase) != 0)
                        {
                            if(debug == 0)
                            {
                                perror("\nError msg\n");
                                printf(bold_start"\n-Failed to Remove File-\n"bold_end);
                            }
                        }  
                        else if(rename(filename_temp,customerdatabase) != 0)
                        {
                            if(debug == 0)
                            {
                                perror("\nError msg\n");
                                printf(bold_start"\n-Failed to Replace File-\n"bold_end);
                            }
                        }
                        else
                        {
                            printf(bold_start"\n-Meter Surrendered for Premises: %s-\n"bold_end,user.premisesID[premises_pick]);
                            audit_editcustomer(12,id_of_current_account);
                        }
                    }
                    else
                    {
                        printf(file_open_error);
                    }
                }

            }
            else
            {
                printf(text_red_start"\nNo Premises Owned by this account\n"text_color_end);
            }
            break;
        case 0:
            system(terminal_clear_string);
            close_console(delay_time,print_delay);
            break;
    }

    printf("\nWhen you're done enter (X):\n");
    scanf(" %c", &send_back_variable);

    if(send_back_variable == 'X' || send_back_variable == 'x')
    {
        goto jump_customer_actions;
    }

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
    else
    {
        printf(file_open_error);
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
    char strtemp[max_e_length];
            
    //attempts to open login Database file
    fp = fopen(filename, "a");
    if(fp != NULL)
    {
        if(add_type != 1 )//appends data passed through into file opened previously ^
        {
            fputs(id_prefix, fp);
            fputs(customerID, fp);
            fputs("\n", fp);
                    
            fputs("User Email: ", fp);
            fputs(user_email, fp);
            fputs("\n", fp);
                    
            fputs(password_prefix, fp);
            
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
    
    srand(time(NULL)); //Sets random function's seed to current time
    int min = 1;
    int max = 5;
    
    int income_class1  = min + rand() % ((max - min)+1); // Assigns a random integer ( from max to min ) to variable

    //attempts to open Customer Database file
    fp = fopen(filename2, "a");
    if(fp != NULL)
    {
        //appends data passed through into file opened previously ^
        fputs(id_prefix, fp);
        fputs(customerID, fp);
        fputs("\n", fp);

        fputs(status_prefix, fp);
        fputs("ACTIVE", fp);
        fputs("\n", fp);

        snprintf(strtemp,max_e_length,income_class_prefix"%d",income_class1);
        fputs(strtemp,fp);
        fputs("\n", fp);

        fputs(fname_prefix, fp);
        fputs(user_firstname, fp);
        fputs("\n", fp);
        
        fputs(lname_prefix, fp);
        fputs(user_lastname, fp);
        fputs("\n", fp);

        fputs(cycle_prefix, fp);
        fputs("0", fp);
        fputs("\n", fp);

        fputs(balance_overdue_prefix, fp);
        fputs("0", fp);
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
int user_login (char *email, char *password, char* tempID, int* is_user_new)
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
            if(strstr(str,id_prefix) != NULL && login_validation == 1)//An id prefix FOUND
            {
                position1 = ftell(fp); // Saves where ID was found as character index
                
                strcpy(tmpID_hold,str);
                //Sanitizing values ( removing new line values, carriages, white space and other unwanted characters)
                strsanitize(tmpID_hold,1);

                if(debug == 0)
                {

                }
            }
            
            if(strstr(str,email_prefix) != NULL && strstr(str, email) != NULL)//If email is found
            {
                //Assigns email found at ^ statement to "found_email"
                char *found_email = strstr(str, email);
                
                if (found_email != NULL)
                {
                    strcpy(temp_email, found_email);
                    
                    if (debug == 0)
                    {
                        printf("\n\n=========== DEBUG DATA ===========\n");
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
                            printf("\n==================================\n");
                            
                        }

                    }
                
                }
                
                if(strcmp(temp_email,admin_email) == 0 && strcmp(temp_password,hashed_password) == 0)
                {
                    login_validation = 2; //Successful admin login
                }
                else if(strcmp(temp_email,email) == 0 && strcmp(temp_password,hashed_password) == 0)
                {
                    login_validation = 0; //Successful customer login
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
    char *ptr;
    if(login_validation == 0 || login_validation == 2)
    {
        if(fseek(fp,position1,SEEK_CUR)==0)//Brings file pointer 1 line before UserID associated with user credentials
                                           //if statement only continues if fseek was successful in moving pointer (returned 0)
        {
            if(fgets(str,max_e_length,fp) != NULL)//Goes down by online and stores string found in "str"
            {
                strcpy(tmpID_hold,str);
            }

            ptr = tmpID_hold;

            //Sanitizing string ( removing new line values(\n), carriages(\r), white space(" ") and other unwanted characters)
            if(strstr(ptr,id_prefix)!=NULL)//If string to be deleted is found
            {
                for(int i = 0; i<(strlen(ptr)); i++)
                {
                    // "UserID: 5343075" ->> "5343075"
                    *(ptr+i) = *(ptr+(i+(strlen(id_prefix))));
                }
            }

            strcpy(tempID,ptr);

            if (debug == 0)
            {
                printf("\n\nRaw string from txt file (ID lookup): %s\n",tmpID_hold);
                printf("UserID character location: %ld\n", position1);
                printf("ID sent to audit function: %s\n",tempID);
            }
        }
        else if(fseek(fp,position1,SEEK_CUR) != 0 && debug == 0)
        {
            printf("\nfseek error\n");
        }

        if(login_validation == 0 || login_validation == 2)
        {
            clear_stringarray(str);
            clear_stringarray(temp_password);

            strcpy(temp_password,ptr); //Copies ID found in previous lines into temp_password
            strcat(temp_password,"@nzhi");  //Adds @nzhi to the end of ^
            hash_djb2(temp_password,str); // Hashes ^
            
            if(debug == 0)
            {
                printf(underline_start"\nChecking if user is using default password\n"underline_end);
                printf("PASSWORD FROM USER BEING CHECKED: %s\n",hashed_password);
                printf("EXPECTED PASSWORD (IF USER IS NEW): %s\n",str);
            }

            
            if(strstr(hashed_password,str) != NULL) // Checks if the account has a password that would 
                                                    // be the default given when account is created in admin terminal
            {
                if(debug == 0)
                {
                    printf("USER STATUS: NEW\n");
                }
                *is_user_new = 0; // Set to true  
            }
            else
            {
                if(debug == 0)
                {
                    printf("USER STATUS: NOT NEW\n");
                }
            }
        }
    }

    fclose(fp);
    ptr = NULL;
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
        printf("\n\nFUNCTION GENERATE ID TEST: %lld\n\n", ID);
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
                printf("\nMAX password length (%d) exceeded. Preceding characters saved, following characters discarded",password_length);
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
    
    //Temporarily saving hash integer as string for checks used in padding
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

    //Assigns last value in char array a null terminator
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
            
        // Format MONTH - DAY - YEAR | HOUR - MINUTE
        snprintf(log.date,45,"[%02d/%02d/%d] - [%02d:%02d]", date.tm_mon +1 , date.tm_mday, date.tm_year + 1900, date.tm_hour, date.tm_min);

        if (audit_type == 0) //Customer was added successfully
        {
            fprintf(fp,"%s | Account-created-(by-customer) | ID-created: %s\n",log.date,customerID);
        }
        else if (audit_type == 1) //Customer was added successfully by admin
        {
            fprintf(fp,"%s | Account-created-(by-admin) | ID-created: %s\n",log.date,customerID);
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
                
        // Format MONTH - DAY - YEAR | HOUR - MINUTE
        snprintf(log.date,45,"[%02d/%02d/%d] - [%02d:%02d]", date.tm_mon +1 , date.tm_mday, date.tm_year + 1900, date.tm_hour, date.tm_min);

        if(audit_type == 0) //Customer logged in successfully
        {
            fprintf(fp,"%s | Account-login-(by-customer) | ID-of-user: %s\n",log.date,customerID);
        }
        else if (audit_type == 2) //Admin logged in successfully
        {
            fprintf(fp,"%s | Account-login-(by-admin) | ID-of-user: %s\n",log.date,customerID);
        }

        fclose(fp);
    }
    else
    {
        printf(file_open_error);
    }


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
    if(strlen(string_tobe_cleared)>0)
    {
        for(int i = 0; i<strlen(string_tobe_cleared); i++)
        {
            *(string_tobe_cleared+i) = 0;
        }
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
                
        // Format MONTH - DAY - YEAR | HOUR - MINUTE
        snprintf(log.date,45,"[%02d/%02d/%d] - [%02d:%02d]", date.tm_mon +1 , date.tm_mday, date.tm_year + 1900, date.tm_hour, date.tm_min);

        switch (audit_type)
        {
            case 1: //Edit made to customer email
                fprintf(fp,"%s | Edit-made-to-customer-income-class-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 2: //Edit made to customer 
                fprintf(fp,"%s | Edit-made-to-customer-email-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 3: //Edit made to customer first name
                fprintf(fp,"%s | Edit-made-to-customer-first-name-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 4: //Edit made to customer last name
                fprintf(fp,"%s | Edit-made-to-customer-last-name-(by-admin)-| ID-of-affected-user: ",log.date);
                break;
            case 5: //Edit made to customer meter size
                fprintf(fp,"%s | Edit-made-to-customer-meter-size-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 6: //Edit made to customer meter reading
                fprintf(fp,"%s | Edit-made-to-customer-meter-reading-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 7: //Edit made to all customer fields
                fprintf(fp,"%s | Edit-made-to-all-customer-fields-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 8: //Customer account archived
                fprintf(fp,"%s | Customer-account-archived-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 9: //Edit made to customer password by customer
                fprintf(fp,"%s | Edit-made-to-customer-password-(by-customer) | ID-of-affected-user: ",log.date);
                break;
            case 10: //Edit made to customer password by admin
                fprintf(fp,"%s | Edit-made-to-customer-password-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 11: //Premises added to customer account by admin
                fprintf(fp,"%s | Premises-added-to-customer-account-(by-admin) | ID-of-affected-user: ",log.date);
                break;
            case 12: //Customer surrendering meter
                fprintf(fp,"%s | Meter/Premises-surrendered-(by-customer) | ID-of-affected-user: ",log.date);
                break;
        }

        fprintf(fp,"%s\n",customerID);
        fclose(fp); 
    }
    else
    {
        printf(file_open_error);
    }


}

//18. Function defintion
// 0 if user is Active
// 1 if user is Archived
// 2 if user could not be found
int get_user_status(char* customerID_for_lookup)
{
    char str[max_e_length];
    char strtemp[max_e_length];
    int found_databreakpoint = 1;
    int found_ID = 1; // Default to false
    int user_status = 2;

    FILE *fp;
    fp = fopen(customerdatabase,"r");

    if(fp != NULL)
    {
        while(fgets(str,max_e_length,fp) != NULL && found_databreakpoint != 0)
        //While there's something to read in file,
        {

            if(strstr(str,customerID_for_lookup) != NULL)
            {
                found_ID = 0;
            }
            if(found_ID == 0 && found_databreakpoint != 0)
            {
                if(strstr(str,status_prefix)!=NULL)
                {
                    strcpy(strtemp,str);
                    strsanitize(strtemp,0);
                    remove_prefix(strtemp,status_prefix);

                    if(strcmp(strtemp,"ACTIVE") == 0)
                    {
                        user_status = 0;
                        break;
                    }
                    else if (strcmp(strtemp,"ARCHIVED") == 0)
                    {
                        user_status = 1;
                        break;
                    }
                }
            }
            if(found_ID == 0 && strstr(str,data_breakpoint)!=NULL)
            {
                found_databreakpoint = 0;
                break;
            }
        }

        fclose(fp);
    
    }
    else
    {
        printf(file_open_error);
    }

    return user_status;
}

//19. Function definition ( String has character )
// 0 if letter was found
// 1 if letter was not found
int strhchar(char *string_to_check)
{
    int i = 0;
    while(*(string_to_check+i) != '\0')
    // while end of string has not been reached, do:
    {
        if(isalpha(*(string_to_check+i)) != 0)
        {
            return 0;
        }
        i++;
    }
    return 1;
}

//Defintion of function 20.
void setall_uppercase (char* string)
{
    char temp_string[max_e_length] = {0};
    int i;

    strcpy(temp_string, string);

    for(i = 0; i<strlen(temp_string); i++)
    {
        temp_string[i] = toupper(temp_string[i]);
    }
    temp_string[i] = '\0';

    strcpy(string, temp_string);
}

//Defintion of function 21.
// Returns 1 if string has no numbers
// Returns 0 if string has numbers
int strhint(char *string_to_check)
{
    int i = 0;
    while(*(string_to_check+i) != '\0')
    // while end of string has not been reached, do:
    {
        if(isdigit(*(string_to_check+i)) != 0)
        {
            return 0;
        }
        i++;
    }
    return 1;
}

//22. Function defintion
// 0 if premises is not available
// 1 if premises is available
// 2 if premises could not be found
int get_premises_status(char* premisesID_for_lookup)
{
    char str[max_e_length];
    char strtemp[max_e_length];
    int found_databreakpoint = 1;
    int found_ID = 1; // Default to false
    int premises_status = 2; // Defaults to ID not being found

    FILE *fp;
    fp = fopen(customerdatabase,"r");

    if(fp != NULL)
    {
        while(fgets(str,max_e_length,fp) != NULL)
        //While there's something to read in file,
        {

            if(strstr(str,premisesID_for_lookup) != NULL)
            {
                found_ID = 0;
            }
            if(found_ID == 0)
            {
                if(strstr(str,premisesid_prefix)!=NULL && strstr(str,surrender_prefix)== NULL) 
                // if id is found NOT surrendered then do:
                {
                    premises_status = 0;
                    break;
                }
                else if(strstr(str,premisesid_prefix)!=NULL && strstr(str,surrender_prefix)!= NULL)
                // if id is found surrendered then do:
                {
                    premises_status = 1;
                }
                
            }
        }

        fclose(fp);
    
    }
    else
    {
        printf(file_open_error);
    }
    return premises_status;
}
