

/* ................................... Header content ......................*/



#ifndef IPV4_PARSER_H
#define IPV4_PARSER_H

#include <stdint.h>                 // NOLINT



#define NULL_TERMINATOR                         ('\0')         /**< Define ASCII character             */     
#define DOT_ASCII                               ('.')          /**< Define ASCII character             */
#define ADDRESS_OFF_PREVIOUS_DOT                (5U)           /**< Character width between two "."    */
#define MAX_NUMBER_OF_DOT                       (3U)           /**< Maximum number of "." in ipv4      */
#define IPV4_SHIFT_OCTET_BIT                    (8U)           /**< Bit width in one octet */
#define IPV4_STRING_LENGHT_MAX                  (16U)          /**< Constraints of string ipv4         */         
#define SHIFT_DECIMAL                           (10)           /**< Radix used for decimal accumual    */
#define MAX_IPV4_OCTET_VALUE                    (255U)         /**< Maximun numeric value of one octet */ 


/**< Converse charater to decimal from '0' to '9' */
#define CHAR_TO_DEC(c)                          (((c) >= '0') && ((c) <= '9') ? ( (c) - '0') : -1)






/**
 * @brief Parser ipv4 string to uint32_t
 * 
 * @param[in] ip_str pointer point to string of ipv4
 * @param[out] p_ip_out pointer point to result of ipv4 type uint32
 * @return int8_t 
 * @retval  0 parse successful
 * @retval -1 out of range or NULL input
 */
int8_t parse_ipv4(const char *p_ip_str, uint32_t *p_ip_out);




#endif /*IPV4_PARSER_H*/
