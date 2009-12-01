import min_ext
import sut_ext


def case_min():
        """Print to CUI"""
        min_ext.Print_to_cui("PYTHON")
	return 0;

def case_sut():
        """Create file"""
        min_ext.Print_to_cui("file creation")
        retval = sut_ext.File_creation("/tmp/python_created_file")
        return retval

def case_faulty():
        """div by zero"""
        inte = 37/0
        

def case_error():
        """will it segfault ?"""
        sut_ext.Segmentation_Fault()
        min_ext.Print_to_cui("This should not print")
        return 0;

def case_set_ev():
        """Set Ind. Event"""
        min_ext.Set_indication_event("pyevent")
        min_ext.Print_to_cui("pyevent set (indication)")
        return 0

def case_set_ev2():
        """Set state Event"""
        min_ext.Set_state_event("pyevent2")
        min_ext.Print_to_cui("pyevent2 set (state)")
        return 0

def case_set_evu():
        """Unset state Event"""
        min_ext.Unset_event("pyevent2")
        min_ext.Print_to_cui("pyevent2 unset")
        return 0

def case_try_ev():
        """Test Events indication"""
        
        min_ext.Request_event("pyevent")
        min_ext.Print_to_cui("wait indication event")
        retval = min_ext.Wait_event("pyevent")
        min_ext.Print_to_cui("indication event received")
        min_ext.Release_event("pyevent")
        return retval

def case_try_ev2():
        """Test Events state"""
        min_ext.Request_event("pyevent2","state")
        min_ext.Print_to_cui("wait state event")
        retval = min_ext.Wait_event("pyevent2")
        min_ext.Print_to_cui("state event received")
        min_ext.Release_event("pyevent2")
        return retval

def case_crun():
        """try running"""
        result = min_ext.Complete_case("minDemoModule","Demo_2")
        if (result == 0):
                min_ext.Print_to_cui("case pasased")
        else:
                min_ext.Print_to_cui("case failed")
        return 0;

def case_srun():
        """Async. exec"""
        min_ext.Start_case("minDemoModule","Demo_2")
        return 0
def case_logs():
        """try logger"""
        log = min_ext.Create_logger("/tmp","pythonlog","txt")
        min_ext.Print_to_cui("logger created")
        min_ext.Log(log,'b',"log text")
        min_ext.Print_to_cui("logger used")
        min_ext.Destroy_logger(log);
        min_ext.Print_to_cui("logger destroyed")
        return 0
