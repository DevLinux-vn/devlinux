#ifndef PACKET_UTIL_H
#define PACKET_UTIL_H

/**
 * @brief Error codes.
 */
typedef enum e_error_code
{
    ERR_OK            = 0,  /**< Operation completed successfully. */
    ERR_INVALID_PARAM,      /**< Invalid parameter. */
    ERR_COUNT               /**< Number of supported error codes. Must remain last. */
} e_error_code_t;

/**
 * @brief Commands.
 */
typedef enum e_cmd
{
    MAIN_MENU     = 0,  /**< Main menu drawing command. */
    SETTINGS_MENU,      /**< Settings menu drawing command. */
    ABOUT_MENU,         /**< About menu drawing command. */
    CMD_COUNT           /**< Number of supported commands. Must remain last. */
} e_cmd_t;

/**
 * @brief Dispatch a UI command to the corresponding menu handler.
 *
 * Validates @p menu_index and uses it to select the corresponding handler
 * from the UI dispatch table. If the index is outside the supported range,
 * no handler is called and an error is reported.
 *
 * @param[in] menu_index Index of the UI command to dispatch.
 *
 * @return Result of the dispatch operation.
 * @retval ERR_OK            The menu index is valid and the corresponding
 *                           handler was called successfully.
 * @retval ERR_INVALID_PARAM The menu index is outside the supported range.
 */
e_error_code_t dispatch_ui(const uint8_t menu_index);

#endif