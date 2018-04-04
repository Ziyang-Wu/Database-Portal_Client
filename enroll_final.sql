
delimiter //

DROP function if exists eligible;//
DROP procedure if exists enroll;//


create function eligible ( studId int , c_code char(20), currentS char(20), currentY int, nextS char(20), nextY int) 
returns int
begin 
 
 declare taken int default 1;
 declare prereq int default 0;
 declare offered int default 0;
 declare spot int default 0;
 
 
 
 SET taken = (select count(*) from transcript where UoSCode = c_code and StudId= studId);

 
 
 SET prereq= (select not exists   (select * from  (select * from requires where UoSCode = c_code) R left join transcript T 
					on T.UoSCode = R.PrereqUoSCode and T.StudId =studId
                    where T.Grade = "F" or T.Grade is null or T.StudId is null));


SET offered =(SELECT EXISTS (SELECT * FROM uosoffering WHERE  (c_code = UoSCode AND currentS = Semester AND currentY = Year ) 
					or (c_code = UoSCode AND nextS = Semester AND nextY = Year)));

	

SET spot = (SELECT EXISTS (Select * from uosoffering WHERE Enrollment< MaxEnrollment AND   c_code = UoSCode));
 
 
 IF (taken =0 AND prereq = 1 AND offered =1 AND spot =1 ) THEN return 1;
 
ELSEIF (prereq=0) THEN return 2;
 
else return 0;
 
 END IF;

 
end//
delimiter;

delimiter //
create procedure enroll (IN studId int, IN c_code char(10), IN currentS char(10), IN currentY int, IN nextS char(10), IN nextY int, IN c_semester char(10), IN c_year int, out flagPre int)
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

IF (eligible(studId, c_code, currentS, currentY, nextS, nextY)=1) THEN 

UPDATE uosoffering SET uosoffering.Enrollment= uosoffering.Enrollment+1 WHERE c_code = UoSCode;

Insert INTO transcript (StudId, UoSCode, Semester, Year, Grade) values( studId, c_code, c_semester, c_year, Null);

ELSEIF(eligible(studId, c_code, currentS, currentY, nextS, nextY)=2) then 
SET flagPre =1;

END IF;

COMMIT;

end//

delimiter ;

SET @flagPre=0;
