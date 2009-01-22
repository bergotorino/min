# This file is part of MIN Test Framework. Copyright (c) 2008 Nokia Corporation
# and/or its subsidiary(-ies).
# Contact: Teemu Virolainen
# Contact e-mail: DG.MIN-Support@nokia.com
# 
# This program is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, version 2 of the License. 
#  
# This program is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
# more details. You should have received a copy of the GNU General Public 
# License along with this program. If not,  see 
# <http://www.gnu.org/licenses/>.




#  file:       minwrap.py
#  version:    0.1
#  brief:      This file contains wrapper for PyUnit tests
#              so that they can be executed under control of MIN


#/usr/bin/env python
import unittest

#Import testcase modules here
import PyUnit_Module_XXX

# Grab the module into test suite.
loader = unittest.TestLoader()
# Give imported module's name as an argument for next function
suite = loader.loadTestsFromModule(PyUnit_Module_XXX)

def minname(test):
    """Return name corresponding to min naming scheme"""
    return 'case_'+test._testMethodName.lstrip('test')

def decmin(test):
    """Decorate unittest classes to look like min style methods"""
    def wrap(test=test):
        result = unittest.TestResult()
        test.run(result)
        #MIN considers exceptions to be errors(eg. failures in testcase/enviroment itself)
        if(len(result.errors) > 0):
            raise AssertionError
        #MIN considers 0 to be passed testcase
        if result.wasSuccessful():
            return 0
        #Non zero is considered failed testcase
        else:
            return -1
    wrap.__doc__ = test._testMethodDoc
    wrap.__name__ = minname(test)
    return wrap

def dumptests(suite):
    if isinstance(suite,unittest.TestSuite):
        for i in suite._tests:
            dumptests(i)
    else:
        globals()[minname(suite)] = decmin(suite)

dumptests(suite)
