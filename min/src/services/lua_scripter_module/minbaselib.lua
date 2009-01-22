-- =============================================================================
-- GLOBAL VARIABLES
-- =============================================================================

-- Test Case result
TP_PASSED = 0
TP_FAILED = 1

-- Event type
StateEvent = 1
IndicationEvent = 0

-- Slave type 
SlaveTypePhone="phone"
SlaveTypeTablet="tablet"

-- =============================================================================
-- LUASCRIPTER MODULE INTERNAL FUNCTIONS
-- =============================================================================

function __testcase_lookup(filename,id)
        local fp     = io.open(filename);
        local n      = 0;
        local tcfunc;
        for line in fp:lines() do
                if string.find(line,"^function case_") then
                        if( n == id ) then
                                tcfunc = string.match(line,"case_[%w]+");
                        end
                        n=n+1;
                end
        end
        return tcfunc;
end

--------------------------------------------------------------------------------

function __get_testcases(filename)
        local fpp = io.open(filename);
        if fpp==nil then
                return;
        end
        for line in fpp:lines() do
                if string.find(line,"^function case_") then
                        local marker = string.find(line,")");
                        local tcfunc = string.sub(line,10,marker-2);
                        local tcname = string.sub(line,marker+1);
                        tcname       = string.match(tcname,"[$%a][%w+%s*]+");
                        min.register_testcase(tcfunc,tcname);
                end        
        end
end

--------------------------------------------------------------------------------

