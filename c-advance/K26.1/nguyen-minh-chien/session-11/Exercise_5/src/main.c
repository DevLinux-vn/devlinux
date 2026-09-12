#include "wide_string_utils.h"

#include <locale.h>
#include <stddef.h>
#include <wchar.h>
#include <wctype.h>

/**
 * @brief Program entry point.
 *
 * Demonstrates locale setup, wide strings, wide character
 * classification, wchar_t portability, and wide string operations.
 *
 * @return 0 on success.
 * @return Non-zero on error.
 */
int main(void)
{
    const char *p_locale;

    wchar_t greeting[] = L"Héllo Wörld! π Ω";

    wchar_t copy_buffer[64] = L"";
    wchar_t concat_buffer[64] = L"";

    int copy_result;
    int concat_result;
    int compare_equal;
    int compare_less;

    p_locale = setlocale(LC_ALL, "");

    if (p_locale == NULL)
    {
        return 1;
    }

    wprintf(L"========== L11 Exercise 5: Wide Characters & I18N ==========\n\n");

    wprintf(L"--- Locale ---\n");
    wprintf(L"Active locale: %hs\n\n", p_locale);

    wprintf(L"--- Wide Strings ---\n");
    wprintf(L"Greeting: %ls\n", greeting);

    wprintf(L"wcslen = %zu characters\n",
            wcslen(greeting));

    wprintf(L"sizeof = %zu bytes "
            L"(each wchar_t = %zu bytes on this platform)\n\n",
            sizeof(greeting),
            sizeof(wchar_t));

    wprintf(L"--- Wide Character Classification ---\n");

    wprintf(L"iswalpha(L'Ω') = %ls\n",
            iswalpha(L'Ω') != 0 ? L"YES (alphabetic)"
                                : L"NO");

    wprintf(L"iswdigit(L'5') = %ls\n",
            iswdigit(L'5') != 0 ? L"YES (digit)"
                                : L"NO");

    wprintf(L"iswspace(L' ') = %ls\n\n",
            iswspace(L' ') != 0 ? L"YES (whitespace)"
                                : L"NO");

    wprintf(L"--- Portability Trap ---\n");

    wprintf(L"sizeof(wchar_t) = %zu bytes\n",
            sizeof(wchar_t));

    /*
     * wchar_t is not portable as a serialized/network format.
     *
     * On Linux/macOS, wchar_t is commonly 4 bytes and typically
     * represents Unicode code points directly.
     *
     * On Windows, wchar_t is commonly 2 bytes and uses UTF-16 code units.
     *
     * Therefore, transmitting raw wchar_t arrays between platforms can
     * produce incompatible data layouts.
     *
     * UTF-8 stored in char arrays is commonly preferred for files,
     * protocols, and network data exchange.
     */

    wprintf(L"Note: On Windows wchar_t is commonly 2 bytes. "
            L"Use UTF-8 for network/file exchange.\n\n");

    wprintf(L"--- Wide String Operations ---\n");

    copy_result = wide_string_copy(
        copy_buffer,
        sizeof(copy_buffer) / sizeof(copy_buffer[0]),
        greeting);

    if (copy_result != 0)
    {
        return 2;
    }

    wprintf(L"wcscpy: %ls\n", copy_buffer);

    concat_result = wide_string_copy(
        concat_buffer,
        sizeof(concat_buffer) / sizeof(concat_buffer[0]),
        greeting);

    if (concat_result != 0)
    {
        return 3;
    }

    concat_result = wide_string_concat(
        concat_buffer,
        sizeof(concat_buffer) / sizeof(concat_buffer[0]),
        L" — Status OK");

    if (concat_result != 0)
    {
        return 4;
    }

    wprintf(L"wcscat: %ls\n", concat_buffer);

    compare_equal = wide_string_compare(L"ABC", L"ABC");
    compare_less = wide_string_compare(L"ABC", L"DEF");

    wprintf(L"wcscmp(\"ABC\", \"ABC\") = %d (equal)\n",
            compare_equal);

    wprintf(L"wcscmp(\"ABC\", \"DEF\") < 0 = %ls\n",
            compare_less < 0 ? L"YES" : L"NO");

    return 0;
}