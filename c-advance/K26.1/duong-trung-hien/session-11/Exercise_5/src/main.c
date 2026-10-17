#include "wide_string_utils.h"
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <wctype.h>

#define APP_SUCCESS         (0)
#define BOOL_TO_YES_NO(x)   (x) ? "YES" : "NO"
#define STRINGIFY(x)        #x
#define TO_STRING(x)        STRINGIFY(x)          

int32_t main() {
    const char *p_locale;

    p_locale = setlocale(LC_ALL, "");

    wprintf(L"========== L11 Exercise 5: Wide Characters & I18N ==========\n");

    wprintf(L"\n--- Locale ---\n");
    wprintf(L"Active locale: %s\n", p_locale);

    wprintf(L"\n--- Wide Strings ---\n");
    wchar_t greeting[] = L"Héllo Wörld! π Ω";
    size_t length = wide_string_length(greeting);

    wprintf(L"Greeting: %ls\n", greeting);
    wprintf(L"wcslen = %zu characters\n", length);
    wprintf(L"sizeof = %zu bytes (each wchar_t = %zu bytes on this platform)\n", sizeof(greeting), sizeof(wchar_t));

    wprintf(L"\n--- Wide Character Classification ---\n");
    wprintf(L"%s = %s (alphabetic)\n", TO_STRING(iswalpha(L'Ω')), BOOL_TO_YES_NO(iswalpha(L'Ω')));
    wprintf(L"%s = %s (digit)\n", TO_STRING(iswdigit(L'5')), BOOL_TO_YES_NO(iswdigit(L'5')));
    wprintf(L"%s = %s (whitespace)\n", TO_STRING(iswspace(L' ')), BOOL_TO_YES_NO(iswspace(L' ')));

    wprintf(L"\n--- Portability Trap ---\n");

    /*
    * Portability note:
    * - On Linux/macOS, wchar_t is typically 4 bytes and commonly represents
    *   Unicode code points using UTF-32.
    * - On Windows, wchar_t is 2 bytes and commonly represents UTF-16 code units.
    * - Therefore, wchar_t is not portable for transmitting data over a network.
    *   Use a fixed-width encoding such as UTF-8 instead.
    */
    wprintf(L"sizeof(wchar_t) = %zu bytes\n", sizeof(wchar_t));

    wprintf(L"\n--- Wide String Operations ---\n");
    wchar_t stored[40U];

    wide_string_copy(stored, 40U, greeting);
    wprintf(L"wcscpy: %ls\n", stored);

    wide_string_concat(stored, 40U, L" — Status OK");
    wprintf(L"wcscat: %ls\n", stored);

    wprintf(L"wcscmp(\"ABC\", \"ABC\") = %d (equal)\n", wide_string_compare(L"ABC", L"ABC"));
    wprintf(L"wcscmp(\"ABC\", \"DEF\") = %d (less than)\n", wide_string_compare(L"ABC", L"DEF"));

    return APP_SUCCESS;
}