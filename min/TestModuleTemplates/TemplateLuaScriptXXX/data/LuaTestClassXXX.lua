-- =============================================================================
-- GLOBAL VARIABLES
-- =============================================================================
MOD="/home/user/.min/LuaTestClassXXX.so";

-- =============================================================================
-- TEST CASES
-- =============================================================================

--------------------------------------------------------------------------------

function case_test1() -- Example test case for LuaTestClassXXX
        local a = min.load(MOD);
        local ret = a.Example();
        return ret,"Passed";
end

--------------------------------------------------------------------------------