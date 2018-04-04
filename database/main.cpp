//
//  main.cpp
//  database
//  


#include <time.h>
#include "mysql.h"
#include <iostream> 



using namespace std;
void login(MYSQL *conn);
void menu(int id, MYSQL *conn);
void transcript (int id, MYSQL *conn);
void personalDetails (int id, MYSQL *conn);
void enroll(int id, MYSQL *conn);
void withdraw(int id, MYSQL *conn);
int getQuarter();
int getYear();



int getQuarter(){
    time_t t = time(0);
    int quarter;
    
    tm *ltm = localtime(&t);
    
    if (1+ltm->tm_mon > 8 && 1+ltm->tm_mon < 12){quarter = 1;}
    else if (1+ltm->tm_mon == 12 || 1+ltm->tm_mon < 3){quarter = 2;}
    else if (1+ltm->tm_mon > 2 && 1+ltm->tm_mon < 6){quarter = 3;}
    else if (1+ltm->tm_mon > 5 && 1+ltm->tm_mon < 9){quarter = 4;}
    else return 0;
    return quarter;
}

int getYear(){
    time_t t = time(0);
    int year;
    
    tm *ltm = localtime(&t);
    
    year = 1900 + ltm->tm_year;
    return year;
}


void withdraw(int id, MYSQL *conn){
    int studentID = id;
    char c_code[10];
    char course_semester[10];
    int course_year;
    int oldrow, newrow;
    string option;
    
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    
    char sql[1024];
    //---------------------show courses currently enrolled in----------------------------------------------
    memset(sql, 0x00, 1024);
    sprintf(sql, "select * from transcript where StudId = %d and Grade is null;",studentID);
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    oldrow = (int)mysql_num_rows(res_set); //------------get numrow of transcript before enrolling new course-------
    cout<<"Here are the courses you are currently enrolled in:"<<endl;
    cout<<"StudId "<<"UoSCode   "<<"Semester"<<"Year"<<"Grade"<<endl;
    for (int i=0; i<oldrow; i++){
        row = mysql_fetch_row(res_set);
        if (row != NULL){
            if (row[4]==NULL){
                cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< "NULL" <<endl;
            }
            else{
                cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< row[4]<<endl;
            }
        }
    }
    //-----------initialize the flag for trigger------------------
    memset(sql, 0x00, 1024);
    sprintf(sql, "set @flag = 0"); 
    mysql_query(conn, sql);
    
    
    //-----------------------------------Input course you want to withdraw------------------------------------
    cout<<"Please enter the course code(UoSCode) , year and semester for the course you want to withdraw."<<endl;
    cout<<"1.Enter UoSCode:(Eg.COMP2007)"<<endl;
    cin>>c_code;
    cout<<"2.Enter course year:(Eg. 2018)"<<endl;
    cin>>course_year;
    cout<<"3.Enter course semester:(Eg. Q2)"<<endl;
    cin>>course_semester;
    memset(sql, 0x00, 1024);
    sprintf(sql, "call withdraw (%d, '%s', '%s', %d);", studentID, c_code, course_semester, course_year);
    mysql_query(conn, sql);

    
    
    
    
    //---------------------------get numrow of transcript after withdraw the course---------------------
    memset(sql, 0x00, 1024);
    sprintf(sql, "select * from transcript where StudId = %d and Grade is null;",studentID);
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    newrow = (int)mysql_num_rows(res_set);
    
    if (newrow == oldrow){
        cout<<"withdraw failed! Hahaha"<<endl;
        cout<<"Please try again! Enter 1 if you want to withdraw again. Enter 2 if you want to return to menu. "<<endl;
        
        if (option == "1"){
        withdraw(studentID, conn);
        }
        else {
            menu(studentID, conn);
        }
    }
    else{
        cout<<"Withdraw succeeded!"<<endl;
        
        
        //------------------------------------Trigger for Enrollment below 50%-----------------------
        memset(sql, 0x00, 1024);
        sprintf(sql, "select @flag");
        mysql_query(conn, sql);
        res_set = mysql_store_result(conn);
        row = mysql_fetch_row(res_set);
        if (row != NULL){
            
            if (*row[0] == '1'){
                cout<<"The Enrollment of this course is below 50% of the MaxEnrollment!"<<endl;
                memset(sql, 0x00, 1024);
                sprintf(sql, "set @flag = 0");
                mysql_query(conn, sql);
            }
        }
        //-------------------------------End of trigger-----------------------------------
        
        cout<<"Press 0 to go back to menu. Press any other key to withdraw more courses"<<endl;
        cin>>option;
        
        if (option == "0"){
            menu(studentID, conn);
        }
        
        else {
            withdraw (studentID, conn);
        }
        
        

    }
    
    
}



void enroll(int id, MYSQL *conn){
    int studentID = id;
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    int CYear = getYear(); //current year
    int CQuarter = getQuarter();  //current quarter
    int NYear = CYear + 1;  //next year
    int NQuarter; //next quarter
    string option;
    char c_code[10];
    int course_year;
    char course_semester[3];
    int oldrow, newrow;
    
    char sql[1024];
    memset(sql, 0x00, 1024);
    
    if (CQuarter == 4){
        NQuarter = 1;
    }
    else{
        NQuarter = CQuarter + 1;
    }
    
    sprintf(sql, "select distinct U.* from uosoffering U where (U.Year = %d and U.Semester = 'Q%d') or (U.Year = %d and U.Semester = 'Q%d')",NYear,NQuarter, CYear,CQuarter);
    
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    cout<<"Courses offering in this quarter and next quarter:"<<endl;
    cout<<"UoSCode    "<<"Semester"<<"Year"<<"Textbook    "<<"Enrollment"<<"MaxEnrollment"<<"InstructorId"<<endl;
    for (int i=0; i<numrows; i++){
        row = mysql_fetch_row(res_set);
        if (row != NULL){
            cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< row[4]<<"  "<<row[5]<< "  "<< row[6]<<endl;
        }
    }
    
    
    //---------------------------get numrow of transcript before enrolling new course---------------------
    memset(sql, 0x00, 1024);
    sprintf(sql, "select * from transcript where StudId = %d;",studentID);
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    oldrow = (int)mysql_num_rows(res_set);
  //-----------------------------initialize the flagPre-------------
    memset(sql, 0x00, 1024);
    sprintf(sql, "set @flagPre = 0");
    mysql_query(conn, sql);
    
    //---------------------Input the course which the student want to enroll in--------------
    cout<<"Please enter the course code(UoSCode) , year and semester for the course you want to enroll."<<endl;
    cout<<"1.Enter UoSCode:(Eg.COMP2007)"<<endl;
    cin>>c_code;
    cout<<"2.Enter course year:(Eg. 2018)"<<endl;
    cin>>course_year;
    cout<<"3.Enter course semester:(Eg. Q2)"<<endl;
    cin>>course_semester;
    memset(sql, 0x00, 1024);
    sprintf(sql, "call enroll (%d, '%s', 'Q%d', %d, 'Q%d', %d, '%s', %d,@flagPre);",studentID,c_code, CQuarter,CYear, NQuarter,NYear,course_semester,course_year);
    mysql_query(conn, sql);
    //---------------------------get numrow of transcript after enrolling new course------------------
    memset(sql, 0x00, 1024);
    sprintf(sql, "select * from transcript where StudId = %d;",studentID);
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    newrow = (int)mysql_num_rows(res_set);
    
    if (newrow == oldrow){
        cout<<"Enrollment failed! Hahaha"<<endl;
        memset(sql, 0x00, 1024);
        sprintf(sql, "select @flagPre");
        mysql_query(conn, sql);
        res_set = mysql_store_result(conn);
        row = mysql_fetch_row(res_set);
        if (row != NULL){
            
            if (*row[0] == '1'){
                cout<<"You have not met the pre-requisites: "<<endl;
                memset(sql, 0x00, 1024);
                sprintf(sql, "set @flagPre = 0"); //reset flagPre
                mysql_query(conn, sql);
                
          //------------show pre-requisites---------
                memset(sql, 0x00, 1024);
                sprintf(sql, "select * from requires where UoSCode = '%s'", c_code);
                mysql_query(conn, sql);
                res_set = mysql_store_result(conn);
                numrows = (int)mysql_num_rows(res_set);
                cout<<"UoSCode  "<<"PrereqUoSCode  "<<endl;
                for (int i=0; i<numrows; i++){
                    row = mysql_fetch_row(res_set);
                    if (row != NULL){
                        
                            cout<<row[0]<<"  "<<row[1]<<"  "<<endl;
                        
                        
                    }
                }

                
            }
        }

        
        
        cout<<"Please try again!"<<endl;
    }
    else{
        cout<<"Enrollment succeeded!"<<endl;
    }
    //--------------------show courses already enrolled in------------------------
    memset(sql, 0x00, 1024);
    sprintf(sql, "select * from transcript where StudId = %d and Grade is null;",studentID);
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    numrows = (int)mysql_num_rows(res_set);
    cout<<"Here are the courses you are currently enrolled in:"<<endl;
    cout<<"StudId "<<"UoSCode "<<"Semester  "<<"Year  "<<"Grade"<<endl;
    for (int i=0; i<numrows; i++){
        row = mysql_fetch_row(res_set);
        if (row != NULL){
            if (row[4]==NULL){
                cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< "NULL" <<endl;
            }
            else{
                cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< row[4]<<endl;
            }
        }
    }
    
    cout<<"Press 0 to go back to menu. Press any other key to enroll in more courses"<<endl;
    cin>>option;
    
    if (option == "0"){
        menu(studentID, conn);
    }
    
    else {
        enroll(studentID, conn);
    }
    
}




void personalDetails (int id, MYSQL *conn){
    int studentID = id;

    MYSQL_RES *res_set;
    MYSQL_ROW row;

    char sql[1024];
    memset(sql, 0x00, 1024);
    sprintf(sql, "SELECT * FROM student where Id = %d", studentID);
    
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    row = mysql_fetch_row(res_set);

    
    cout << "ID:"<< row[0]<< endl;
    cout << "Name: "<< row[1]<<endl;
    cout << "Password:"<< row[2]<<endl;
    cout << "Address: "<<row[3]<<endl;
    
    string actionID;
    
    cout << "enter 1 if you want to change address"<<endl;
    cout << "enter 2 if you want to change password"<<endl;
    cout << "enter 3 if you want to go back to menu"<<endl;
    cin >> actionID;
    
    char update[1024];
    memset(update, 0x00, 1024);
    
    
    if (actionID == "1"){
        string newAddress;
        cout << "Please enter new address: "<< endl;
        cin >> newAddress;
        
        sprintf(update, "UPDATE student SET student.Address = '%s' WHERE student.Id= %d", newAddress.c_str(), studentID);

        mysql_query(conn,"start transaction;");
        mysql_query(conn, update);
        mysql_query(conn,"commit;");
        
        res_set = mysql_store_result(conn);
        cout << "The address has been changed" << endl;
        
        personalDetails(studentID, conn);
        
    }
    else if (actionID == "2"){
        string newPassword;
        cout << "Please enter new password: "<< endl;
        cin >> newPassword;
        sprintf(update, "UPDATE student SET student.Password = '%s' WHERE student.Id= %d", newPassword.c_str(), studentID);
        
        mysql_query(conn,"start transaction;");
        mysql_query(conn, update);
        mysql_query(conn,"commit;");
        
        res_set = mysql_store_result(conn);
        cout << "The password has been changed" << endl;
        
        personalDetails(studentID, conn);

    }
    
    else if (actionID == "3"){
        menu( studentID, conn);
    }
    else {
        cout << "Please enter a valid choice!"<< endl;
        personalDetails(studentID, conn);
    }
    
}



void transcript(int id, MYSQL *conn){
    int studentID = id;
    char c_code[8];
    string option;
    
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    
    char sql[1024];
    memset(sql, 0x00, 1024);
    
    sprintf(sql, "SELECT * FROM transcript where StudId = %d", studentID);
    
    
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    
    cout << "studID "<< "UoSCode  "<< " Semester "<< " Year  "<<" Grade"<<endl;
    
    for (int i=0; i<numrows; i++){
        
        row = mysql_fetch_row(res_set);
        if (row != NULL){
            if (row[4]==NULL){
            cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< "NULL" <<endl;
            }
            else{
            cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< row[4]<<endl;
            }
        }
    }
    
    cout<< "enter 1 if you want to go back to menu. enter 2 if you want to see course details.";
    cin>>option;
    if (option == "1"){
        menu(studentID, conn);
    }
    else {
    
    cout<<"To see details of one course, please enter the course number."<<endl;
    cin>>c_code;
        
    memset(sql, 0x00, 1024);
    sprintf(sql, "SELECT StudId, t.UoSCode, t.Year,t.Semester,UoSName, Name, Enrollment, MaxEnrollment, Grade FROM faculty f, transcript t, unitofstudy un,uosoffering uo WHERE StudId =%d and t.UoSCode= '%s' and t.UoSCode=un.UoSCode and t.UoSCode=uo.UoSCode and uo.InstructorId=f.Id and t.Year=uo.Year and t.Semester=uo.Semester", studentID, c_code);
    
    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    numrows = (int)mysql_num_rows(res_set);
    
    if (numrows == 0){
    cout<<"Please enter a valid course number."<<endl;
    transcript(studentID,conn);
    }
    
    
    else {
    
        row = mysql_fetch_row(res_set);
        if (row != NULL){
            cout<<"Details of Course "<<c_code<<": "<<endl;
            cout << "studID "<< "UoSCode  "<< " Year "<< " Semester  "<<" UoSName   "<<"InstructorName  "
            <<"Enrollment  "<<"MaxEnrollment  "<<"Grade"<<endl;
            cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<"   "<< row[4]<<"  "<<row[5]<<"  "<<row[6]<< "  "<< row[7]<<"   "<< row[8]<<endl;
            }
    
        }
    
    
    cout<<"Press any key to go back to transcript"<<endl;
    cin>>option;
    
        transcript(studentID, conn);
    
   
    }
}


void menu(int id, MYSQL *conn){
    int studentID = id;
    int year;
    int quarter;
    string choice;
    
    cout << "Welcome to menu!"<<endl;

    
    year= getYear();
    quarter= getQuarter();
    
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    
    char sql[1024];
    memset(sql, 0x00, 1024);
    sprintf(sql, "SELECT * FROM transcript where StudId = %d  and year = %d and semester= 'Q%d'", studentID,year,quarter);

    mysql_query(conn, sql);
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    cout << "studID "<< "UoSCode "<< "Semester "<< "Year "<<endl;
    for (int i=0; i<numrows; i++){
        row = mysql_fetch_row(res_set);
        if (row != NULL){
        cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<< "  "<< row[3]<<endl;
        }
    }
    cout << "Press 1 to Transcript. Press 2 to Enroll, Press 3 to Withdraw, Press 4 to view Personal Details, Press 5 to Logout"<< endl;
       cin>>choice;
    
    if (choice == "1"){
        transcript(studentID, conn);
    }
    else if (choice == "4"){
        personalDetails(studentID, conn);
    }
    else if (choice == "5") {
        login(conn);
    }
    else if (choice == "2"){
        enroll (studentID, conn);
    }
    else if (choice == "3"){
        withdraw (studentID, conn);
    }

        
    else {
        login(conn);
    }
    
   
}



void login(MYSQL *conn){
    
  
        
        MYSQL_RES *res_set;
        MYSQL_ROW row;
    
        //char UnameInput[10];
        int Uname;
        char PassW[10];
        char sql[1024];
        memset(sql, 0x00, 1024);
        
        cout<<"Log in"<<endl;
        cout<<"Please input your username(must be a number):"<<endl;
        cin>>Uname;
        cout<<"Please input your password:"<<endl;
        cin>>PassW;
        sprintf(sql, "SELECT * FROM student WHERE id = %d and password = '%s'", Uname,PassW);
        
        
        mysql_query(conn,sql);
        res_set = mysql_store_result(conn);
        row = mysql_fetch_row(res_set);
        
        if (res_set == NULL || row==0)
        {
            cout<<"The username and/or password you specified are incorrect, please try again."<<endl;
            login(conn);
        }
        else {
            menu(Uname, conn);
        }
    
}




int main (int argc, const char * argv[]) {
    
    
    MYSQL *conn, mysql;
    mysql_init(&mysql);
    conn = mysql_real_connect(&mysql, "localhost", "root", "karen65040958",
                                    "project3-nudb", 3306, NULL, 0);
    
    login(conn);
    
    mysql_close(conn);
   
}


