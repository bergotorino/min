/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
 * Contact e-mail: DG.MIN-Support@nokia.com
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 2 of the License. 
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
 * more details. You should have received a copy of the GNU General Public 
 * License along with this program. If not,  see 
 * <http://www.gnu.org/licenses/>.
 */


/**
 * GLOBAL VARIABLES SECTION
 */
#ifdef TEST_VAR_DECLARATIONS
minEventIf *e = NULL;
#endif                          /* TEST_VAR_DECLARATIONS */
/**
 * END OF GLOBAL VARIABLES SECTION
 */


/**
 * TEST CASES SECTION
 */
#ifdef TEST_CASES
/**
 * MIN_SETUP defines activities needed before every test case.
 */
MIN_SETUP {
}

/**
 * MIN_TEARDOWN defines activities needed after every test case
 */
MIN_TEARDOWN {
	if (e)
		min_event_destroy (e);
}

/**
 * MIN_TESTDEFINE defines a test case
 *
 */
MIN_TESTDEFINE (Wait Event1)
{
        e = min_event_create ("Event1", EIndication);
        /* 
        ** Register for event 
        */
        e->SetType (e, EReqEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);
        /*
        ** Wait for the event
        */
        e->SetType (e, EWaitEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);

        /*
        ** Release the event
        */
        e->SetType (e, ERelEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);
}

MIN_TESTDEFINE (Set Event1)
{
        e = min_event_create ("Event1", EIndication);
        sleep (3);

        /*
        ** Set the event
        */
        e->SetType (e, ESetEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);
}

MIN_TESTDEFINE (Wait Event2)
{
        e = min_event_create ("Event2", EState);
        /* 
        ** Register for event 
        */
        e->SetType (e, EReqEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);
        /*
        ** Wait for the event
        */
        e->SetType (e, EWaitEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);

        /*
        ** Release the event
        */
        e->SetType (e, ERelEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);

}

MIN_TESTDEFINE (Set Event2)
{
        e = min_event_create ("Event2", EState);

        /*
        ** Set the event
        */
        e->SetType (e, ESetEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);

}

MIN_TESTDEFINE (Unset Event2)
{
        e = min_event_create ("Event2", EState);

        /*
        ** Set the event
        */
        e->SetType (e, EUnsetEvent);
        Event(e);
        MIN_ASSERT_EQUALS (e->event_status_, EventStatOK);
        
}


#endif
/**
 * END OF TEST CASES SECTION
 */
