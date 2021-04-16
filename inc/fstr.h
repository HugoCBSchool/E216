#ifndef __FSTR_H
#define __FSTR_H

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>


#include "util/util.h"
#include "util/linklist.h"
#include "util/table_string.h"

//#include "modules/critere.h"


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/**
 * @brief  
 * @note   
 * @retval 
 */
#define CASH_VARIADE_SENTINEL   "${[SENTINEL]}"
/**
 * @brief  
 * @note   
 * @retval 
 */
#define $(x)                    "${"#x"}"
/**
 * @brief  
 * @note   
 * @retval 
 */
#define $VARIADE(...)             __VA_ARGS__,CASH_VARIADE_SENTINEL
//===============================================================================
/**
 * @brief  
 * @note   
 * @param  dst: the adress of the string buffer to populate
 * @param  fmt: the litteral string of the format template
 * @param  ...: a variade of 2N+1 elements returned by $VARIADE(x)
 *              the value passed to $VARIADE must be a variade of 2N elements in the 
 *              following order/format:(example)
 *                  fstr_cashvariade_formater_dest(
 *                          &buffer
 *                      ,   $(var1)"some text"$(var2)"some other text"$(var3)$(var4)
 *                      ,   $VARIADE(
 *                                  $(var1) ,  string_to_replace_cash_tag_of_var1_with 
 *                              ,   $(var2) ,  string_to_replace_cash_tag_of_var2_with 
 *                              ,   $(var3) ,  "$(var3)replaced by this string in src"
 *                          )
 *                  );     
 * @retval TRUE on success, FALSE otherwise meaning that dst is also unchanged
 */
bool __fstr_cashvariade_formater_dest( str*dst, cstr fmt, ... );
str  __fstr_cashvariade_formater_make(          cstr fmt, ... );
#define fstr_cashvariade_formater_dest(str_p_destination_ref,cstr_format_template,...) (\
    __fstr_cashvariade_formater_dest(\
        str_p_destination_ref,cstr_format_template,__VA_ARGS__\
    )\
)
#define fstr_cashvariade_formater_make(cstr_format_template,...) (\
    __fstr_cashvariade_formater_make(cstr_format_template,$VARIADE(__VA_ARGS__))\
)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/**
 * @brief  
 * @note   
 * @param  src: 
 * @param  dst: 
 * @param  old_str: 
 * @param  new_str: 
 * @retval 
 */
bool fstr_remplacer_dest( str* dst, cstr src, cstr old_str , cstr new_str );
str  fstr_remplacer_make(           cstr src, cstr old_str , cstr new_str );
//==============================================================================
/**
 * @brief  
 * @note   
 * @param  dest: 
 * @param  table: 
 * @param  sep: 
 * @retval 
 */
bool fstr_flatten_table_string_dest(str* dest, h_table_string table, cstr sep);
str  fstr_flatten_table_string_make(           h_table_string table, cstr sep);
/**
 * @brief  
 * @note   
 * @param  dest: 
 * @param  list: 
 * @param  sep: 
 * @retval 
 */
bool fstr_flatten_linklist_dest(str* dest, h_list list, cstr sep);
str  fstr_flatten_linklist_make(           h_list list, cstr sep);
/**
 * @brief  
 * @note   
 * @param  dest: 
 * @param  list: 
 * @param  sz: 
 * @param  sep: 
 * @retval 
 */
bool fstr_flatten_array_dest(str* dest, cstr* list, uint sz, cstr sep);
str  fstr_flatten_array_make(           cstr* list, uint sz, cstr sep);
//==============================================================================



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



#endif