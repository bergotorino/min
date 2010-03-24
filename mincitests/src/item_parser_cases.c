/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
 * Contact e-mail: min-support@lists.sourceforge.net
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
 *  @file       item_parser_cases.c
 *  @version    0.1
 *  @brief      This file contains test for MIN Item Parser
 */

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <min_common.h>
#include <min_item_parser.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

/** Define macros of implementation testing for print output enabling */
/*
#define CHECK_STRING_DEBUG_PRINTF_OUTPUT
#define CHECK_INT_DEBUG_PRINTF_OUTPUT
#define CHECK_UINT_DEBUG_PRINTF_OUTPUT
#define CHECK_CHAR_DEBUG_PRINTF_OUTPUT
#define CHECK_REMAINDER_DEBUG_PRINTF_OUTPUT
*/
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ============================= TESTS ===================================== */
/* ------------------------------------------------------------------------- */
#ifdef TEST_CASES
MIN_TESTDEFINE(test_mip_create_ok)
{
        MinItemParser* mip  = INITPTR;
        TSChar section[]      = "TEST SECTION";
        int   start_pos      = 0;
        int   length         = strlen( section );
        TSChar check_string[] = "TEST SECTION";
        int  result          = 0;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC (mip, INITPTR, "SIP creation failed." );

        result = strncmp( check_string, mip->item_line_section_,
                strlen( mip->item_line_section_ ) );
        MIN_ASSERT_EQUALS_DESC( result, 0, 
                "SIP parsing result string incorrect." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_create_initptr)
{
        MinItemParser* mip = INITPTR;
        TSChar* section      = INITPTR;
        int    start_pos    = 0;
        int    length       = 4;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR,
				"SIP creation with section INITPTR failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_create_null)
{
        MinItemParser* mip = INITPTR;
        TSChar* section      = NULL;
        int    start_pos    = 4;
        int    length       = 10;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR,
                "SIP creation with section NULL failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_create_start_pos_negative)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "TEST SECTION";
        int   start_pos     = -1;
        int   length        = strlen( section );
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR,
                "SIP creation with negative start position failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_create_start_pos_over_length)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "TEST SECTION";
        int   length        = strlen( section );
        int   start_pos     = length + 1;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR,
                "SIP creation with start position over length failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_create_length_negative)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "TEST SECTION";
        int   start_pos     = 0;
        int   length        = -1;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR,
                "SIP creation with negative length failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_parse_start_and_end_pos_ok)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "COMMENTS1 TAG = KEYWORD2 KEYWORD3";
        int   start_pos     = 0;
        int   length        = strlen( section );
        
        TSChar    start_tag[]       = "TAG";
        TSChar*   ref_start_pos     = INITPTR;
        TSChar*   ref_end_pos       = INITPTR;
        int     ref_length         = 0;
        TSChar*   ref_extra_end_pos = INITPTR;
        TSChar    check_string[]    = "KEYWORD2";
        int     retval             = 0;        
        int     result             = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_parse_start_and_end_pos( mip, start_tag,
                &ref_start_pos, &ref_end_pos, &ref_length,
                &ref_extra_end_pos );
        MIN_ASSERT_EQUALS_DESC( retval, 0,
                "SIP parsing start and end position failed." );

        result = strncmp( check_string, ref_start_pos, ref_length );
        MIN_ASSERT_EQUALS_DESC( result, 0, 
                "SIP parsing result string incorrect." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_parse_start_and_end_pos_initptr)
{
        MinItemParser* mip      = INITPTR;
        TSChar  start_tag[]       = "TAG";
        TSChar* ref_start_pos     = INITPTR;
        TSChar* ref_end_pos       = INITPTR;
        int    ref_length        = 0;
        TSChar* ref_extra_end_pos = INITPTR;
        int    retval            = 0;        

        retval = mip_parse_start_and_end_pos( mip, start_tag,
                &ref_start_pos, &ref_end_pos, &ref_length,
                &ref_extra_end_pos );
        MIN_ASSERT_EQUALS_DESC( retval, -1,
                "SIP parsing with mip INITPTR failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_parse_start_and_end_pos_only_tag)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "   ONLY_TAG = ";
        int       start_pos = 0;
        int       length    = strlen( section );

        TSChar   start_tag[]       = "ONLY_TAG";
        TSChar*  ref_start_pos     = INITPTR;
        TSChar*  ref_end_pos       = INITPTR;
        int     ref_length        = 0;
        TSChar*  ref_extra_end_pos = INITPTR;
        int     retval            = 0;        

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_parse_start_and_end_pos( mip, start_tag,
                &ref_start_pos, &ref_end_pos, &ref_length,
                &ref_extra_end_pos );
        MIN_ASSERT_EQUALS_DESC( retval,-1,
                "SIP parsing with mip only tag failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "TAG1=EXAMPLE_STRING1 TAG2=EXAMPLE_STRING2 TAG3=EXAMPLE_STRING3";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar    tag[]          = "TAG2";
        TSChar*   string         = INITPTR;
        TSChar    check_string[] = "EXAMPLE_STRING2";             
        int      retval         = 0;        
        int      result         = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP parsing operation failed." );
       
        result = strncmp( check_string, string, strlen( string ) );
        MIN_ASSERT_EQUALS_DESC( result, 0, 
                "SIP get string failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf("mip_get_string() string = %s\n", string);
#endif

        if ( string != INITPTR ) DELETE( string );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string_by_white_space_delimiter)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "EXAMPLE_STRING1 EXAMPLE_STRING2 EXAMPLE_STRING3 EXAMPLE_STRING4";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar    tag[]          = "EXAMPLE_STRING2";
        TSChar*   string         = INITPTR;
        TSChar    check_string[] = "EXAMPLE_STRING3";             
        int      retval         = 0;        
        int      result         = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP parsing operation failed." );
       
        result = strncmp( check_string, string, strlen( string ) );
        MIN_ASSERT_EQUALS_DESC( result, 0, 
                "SIP get string by white spaces failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf("mip_get_string() string = %s\n", string);
#endif

        if ( string != INITPTR ) DELETE( string );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string_initptr)
{
        MinItemParser* mip = INITPTR;
        TSChar  tag[]        = "TAG";
        TSChar* string       = INITPTR;             
        int    retval       = 0;        

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, -1, 
                "SIP get string INITPTR failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string_length_zero)
{
        MinItemParser* mip = INITPTR;
        TSChar  section[]    = "    TAG= EXAMPLE_STRING  ";
        int    start_pos    = 0;
        int    length       = strlen( section );
 
        TSChar  tag[]        = "TAG";
        TSChar* string       = INITPTR;             
        int    retval       = 0;        

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
				    "SIP creation failed." );

        *(mip->item_line_section_) = '\0';

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, -1, 
                "SIP get string with length zero failed." );
 
        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string_with_backlashes)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "TAG1 = \"EXAMPLE_STRING1\"  TAG2 = \\\\EXAMPLE_STRING2";
        int   start_pos     = 0;
        int   length        = strlen( section );
 
        TSChar  tag[]          = "TAG2";
        TSChar* string         = INITPTR;
        TSChar  check_string[] = "EXAMPLE_STRING2";
        int    retval         = 0;
        int    result         = 0;        

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get string parsing failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf("mip_get_string() string = %s\n", string);
#endif

        result = strncmp( check_string, string, strlen( string ) );
        MIN_ASSERT_EQUALS_DESC( result, 0, 
                "SIP get string by backslashes parsing failed." );

        if ( string != INITPTR ) DELETE( string );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string_with_backlashes_without_comments)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "TAG1 = \"EXAMPLE_STRING1\"  TAG2 = \\\\  ";
        int   start_pos     = 0;
        int   length        = strlen( section );
 
        TSChar  tag[]        = "TAG2";
        TSChar* string       = INITPTR;
        int    retval       = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, -1, 
                "SIP get string parsing without backslashes comments failed." );

        if ( string != INITPTR ) DELETE( string );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string_with_quotes)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "TAG1 = \"EXAMPLE_STRING1}\"  TAG2 = \"EXAMPLE_STRING2\"";
        int   start_pos     = 0;
        int   length        = strlen( section );
 
        TSChar   tag[]          = "TAG2";
        TSChar*  string         = INITPTR;             
        TSChar   check_string[] = "EXAMPLE_STRING2";
        int     retval         = 0;
        int     result         = 0;        

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
				    "SIP creation failed." );
	mip_set_parsing_type (mip, EQuoteStyleParsing);

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
				"SIP get string parsing failed." );

        result = strncmp( check_string, string, strlen( string ) );
        MIN_ASSERT_EQUALS_DESC( result, 0, 
				"SIP get string by quotes parsing failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf("mip_get_string() string = %s\n", string);
#endif
        if ( string != INITPTR ) DELETE( string );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_string_with_first_quote)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "TAG1 = \"EXAMPLE_STRING1   TAG2 = \"EXAMPLE_STRING2\"";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar  tag[]          = "TAG1";
        TSChar* string         = INITPTR;             
        TSChar  check_string[] = "\"EXAMPLE_STRING1";
        int    retval         = 0;        
        int    result         = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );
	mip_set_parsing_type (mip, ENormalParsing);

        retval = mip_get_string( mip, tag, &string ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get string parsing failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT 
        printf("mip_get_string() string = %s\n", string);
        printf("DEBUG: length = %d\n", strlen( string ) );
#endif

        result = strncmp( check_string, string, strlen( string ) );
        MIN_ASSERT_EQUALS_DESC( result, 0, 
                "SIP get string by first quote parsing failed." );

        if ( string != INITPTR ) DELETE( string );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_next_string)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "TAG1 = EXAMPLE_STRING1  TAG2 = EXAMPLE_STRING2  \\\\ EXAMPLE_STRING3";
        int   start_pos     = 0;
        int   length        = strlen( section );
 
        TSChar  tag[]        = "TAG2";
        TSChar* string[2];             
        int    retval       = 0;        

        string[0] = INITPTR;
        string[1] = INITPTR;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_string( mip, tag, &string[0] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get string failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_string() string = %s\n", string[0] );
#endif
        retval = mip_get_next_string( mip, &string[1] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get string failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_string() string:\n%s\n", string[1] );
#endif

        if ( string[0] != INITPTR ) DELETE( string[0] );
        if ( string[1] != INITPTR ) DELETE( string[1] );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_next_tagged_string)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "TAG = EXAMPLE_STRING1 EXAMPLE_STRING2 TAG = EXAMPLE_STRING3";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar   tag[]      = "TAG";
        TSChar*  string[2];             
        int     retval     = 0;        

        string[0] = INITPTR;
        string[1] = INITPTR;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_string( mip, tag, &string[0] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get string failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_string() string = %s\n", string[0] );
#endif

        retval = mip_get_next_tagged_string( mip, tag, &string[1] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get string failed." );

#ifdef CHECK_STRING_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_tagged_string() string:\n%s\n", string[1] );
#endif

        if ( string[0] != INITPTR ) DELETE( string[0] );
        if ( string[1] != INITPTR ) DELETE( string[1] );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_int)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "EXAMPLE_STRING2 TAG = 123 EXAMPLE_STRING3";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar tag[]  = "TAG";
        int   value  = 0;     
        int   retval = 0;        

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_int( mip, tag, &value ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get integer failed." );

        /* For testing...
        printf( "mip_get_int() value = %d\n", value );
        */

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_next_int)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "EXAMPLE_STRING2 TAG = 123  3456  ";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar tag[] = "TAG";
        int   value[2];
        int   retval;        

        value[0] = 0;
        value[1] = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
				    "SIP creation failed." );

        retval = mip_get_int( mip, tag, &value[0] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get integer failed." );

#ifdef CHECK_INT_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_int() value = %d\n", value[0] );
#endif
        
        retval = mip_get_next_int( mip, &value[1] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get next integer failed." );
        
#ifdef CHECK_INT_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_int() value = %d\n", value[1] );
#endif

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_next_tagged_int)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "EXAMPLE_STRING2 TAG = 123  3456  TEST TAG = 7890";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar tag[] = "TAG";
        int   value[2];
        int   retval;        

        value[0] = 0;
        value[1] = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_int( mip, tag, &value[0] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get integer failed." );

        retval = mip_get_next_tagged_int( mip, tag, &value[1] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get next tagged integer failed." );

#ifdef CHECK_INT_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_tagged_int() value = %d\n", value[1] );
#endif
 
        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_uint_functions)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "EXAMPLE_STRING2  TAG = 123  3456  TEST TAG = 7890";
        int       start_pos = 0;
        int       length    = strlen( section );
 
        TSChar tag[] = "TAG";
        unsigned int value[3];
        int retval;        

        value[0] = 0;
        value[1] = 0;
        value[2] = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_uint( mip, tag, &value[0] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get unsigned integer failed." );
        
#ifdef CHECK_UINT_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_uint() value = %d\n", value[0] );        
#endif

        retval = mip_get_next_uint( mip, &value[1] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get next unsigned integer failed." );

#ifdef CHECK_UINT_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_uint() value = %d\n", value[1] );
#endif

        retval = mip_get_next_tagged_uint( mip, tag, &value[2] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get next tagged unsigned integer failed." );

#ifdef CHECK_UINT_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_tagged_uint() value = %d\n", value[2] );
#endif        

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_char_functions)
{
        MinItemParser* mip = INITPTR;
        TSChar section[]     = "   EXAMPLE_STRING1 TAG=ABC  EXAMPLE_STRING2 TEST TAG=CDEF";
        int   start_pos     = 0;
        int   length        = strlen( section );
 
        TSChar tag[] = "TAG";
        TSChar chr[3];
        int   retval;        

        chr[0] = 0;
        chr[1] = 0;
        chr[2] = 0;

        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        retval = mip_get_char( mip, tag, &chr[0] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get char failed." );

#ifdef CHECK_CHAR_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_char() chr = %c\n", chr[0] );        
#endif

        retval = mip_get_next_char( mip, &chr[1] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get next char failed." );

#ifdef CHECK_CHAR_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_char() chr = %c\n", chr[1] );
#endif

        retval = mip_get_next_tagged_char( mip, tag, &chr[2] ); 
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get next tagged char failed." );

#ifdef CHECK_CHAR_DEBUG_PRINTF_OUTPUT
        printf( "mip_get_next_tagged_char() chr = %c\n", chr[2] );
#endif

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_remainder)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = "       EXAMPLE_STRING1  EXAMPLE_STRING2   ";
        int       start_pos = 0;
        int       length    = strlen( section );
        TSChar*    string    = INITPTR;
        int       retval    = 0;
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

        mip->item_skip_and_mark_pos_ = mip->item_line_section_;

        retval = mip_get_remainder( mip, &string );
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP get remainder failed." );

#ifdef CHECK_REMAINDER_DEBUG_PRINTF_OUTPUT
        printf("mip_get_remainder = %s\n", string );
#endif

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_set_parsing_type)
{
        MinItemParser* mip = INITPTR;
        TSChar     section[] = " TAG = EXAMPLE_STRING1 ";
        int       start_pos = 0;
        int       length    = strlen( section );
        int       retval    = 0;


       retval = mip_set_parsing_type( INITPTR, EQuoteStyleParsing );
       MIN_ASSERT_EQUALS_DESC( retval, -1, 
                "SIP set parsing type with mip INITPTR failed." );
        
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );

       retval = mip_set_parsing_type( mip, EQuoteStyleParsing );
       MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP set parsing type failed." );
        
       MIN_ASSERT_EQUALS_DESC( mip->parsing_type_, EQuoteStyleParsing, 
                "SIP set parsing type to Quete Style Parsing failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_mip_get_parsing_type)
{
        MinItemParser* mip       = INITPTR;
        TSChar           section[] = " TAG = EXAMPLE_STRING1 ";
        int             start_pos = 0;
        int             length    = strlen( section );
        TParsingType    type      = ENormalParsing;
        int             retval    = 0;

        type = mip_get_parsing_type( INITPTR );
        MIN_ASSERT_EQUALS_DESC( type, -1, 
                "SIP get parsing type with mip INITPTR failed." );
                
        mip = mip_create( section, start_pos, length );
        MIN_ASSERT_NOT_EQUALS_DESC( mip, INITPTR, 
                "SIP creation failed." );
        
        retval = mip_set_parsing_type( mip, EQuoteStyleParsing );
        MIN_ASSERT_EQUALS_DESC( retval, 0, 
                "SIP set parsing type failed." );

        type = mip_get_parsing_type( mip );
        MIN_ASSERT_EQUALS_DESC( type, EQuoteStyleParsing, 
                "SIP get parsing test Quete Style Parsing value failed." );

        mip_destroy( &mip );
        MIN_ASSERT_EQUALS_DESC( mip, INITPTR, "SIP destroying failed." );
}
#endif /* TEST_CASES */
