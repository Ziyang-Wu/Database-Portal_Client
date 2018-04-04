delimiter //

DROP function if exists canWithdraw;//
DROP procedure if exists withdraw//


create function canWithdraw ( studId int , c_code char(20)) 
returns int
begin 
 
 declare inProgress int default 0;

 
 SET inProgress = (SELECT exists (SELECT *FROM transcript WHERE  StudId=studId AND UoSCode=c_code AND Grade is null));
 
  
return inProgress;

 end //
delimiter;

delimiter //
create procedure withdraw(IN studId int, IN c_code char(10),  IN c_semester char(10), IN c_year int)
begin 

DECLARE exit handler for SQLEXCEPTION
        BEGIN
        
        ROLLBACK;
        
    END;
    
DECLARE exit handler for SQLWARNING
        BEGIN
        
        ROLLBACK;
        
		END;

START TRANSACTION;

IF (canWithdraw ( studId, c_code) =1) THEN 

UPDATE uosoffering SET uosoffering.Enrollment= uosoffering.Enrollment-1 WHERE c_code = UoSCode;

delete FROM transcript WHERE StudId=studId and UoSCode=c_code and year=c_year and Semester= c_semester and grade is null;

END IF;

commit;

end//

delimiter ;

call withdraw (3213, "COMP5045", "Q1", 2017);


