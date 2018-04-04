delimiter //
DROP procedure if exists needWarning;//
DROP trigger if exists LowEnrollment;//

create procedure needWarning( out flag int)

BEGIN

set flag =1;

END//
DELIMITER ;



DELIMITER //
CREATE TRIGGER LowEnrollment
    AFTER UPDATE ON uosoffering
    FOR EACH ROW 
    
    BEGIN 	

	
IF (NEW.Enrollment < (NEW.MaxEnrollment*0.5)) 
	THEN call needWarning(@flag);
    
    end if;
   
    
	end//
 DELIMITER ;
 
 set @flag=0;

 select @flag;