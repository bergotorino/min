import min_ext

def case_print():
        """Print something"""
        min_ext.Print_to_cui("Hola Mundo!")
	return 0;

def case_faulty():
        """div by zero"""
        inte = 37/0

def case_crun():
        """run Demo_2 case, wait for compelete"""
        result = min_ext.Complete_case("minDemoModule","Demo_2")
        if (result == 0):
                min_ext.Print_to_cui("Demo_2 passed")
        else:
                min_ext.Print_to_cui("Demo_2 failed")
        return result;

def case_srun():
        """Start Demo_2 with asynchronous execution"""
        min_ext.Start_case("minDemoModule","Demo_2")
        return 0;

def case_logs():
        """Use min logger from python"""
        log = min_ext.Create_logger("/tmp","pythonlog","txt")
        min_ext.Print_to_cui("logger created")
        min_ext.Log(log,'b',"log text")
        min_ext.Print_to_cui("logger used")
        min_ext.Destroy_logger(log);
        min_ext.Print_to_cui("logger destroyed")
        return 0;

def case_event_unset():
        """Unset state event (testevent)"""
        min_ext.Unset_event("testevent")
        min_ext.Print_to_cui("testevent unset")
        return 0

def case_set_state_event():
        """Set state Event (testevent)"""
        min_ext.Set_state_event("testevent")
        min_ext.Print_to_cui("testevent set (state)")
        return 0

