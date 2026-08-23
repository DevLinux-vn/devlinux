/**
 * @file main.c
 * @brief Demonstration of Wide Characters, I18N, and Locale setup.
 */
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include "wide_string_utils.h"

int main(void)
{
    /* 1. Locale Setup */
    const char *p_loc = setlocale(LC_ALL, "");

    /* Strictly using wprintf to prevent stream orientation mixing */
    wprintf(L"========== L11 Exercise 5: Wide Characters & I18N ==========\n\n");
    
    wprintf(L"--- Locale ---\n");
    wprintf(L"Active locale: %s\n\n", (p_loc != NULL) ? p_loc : "C");

    /* 2. Wide Strings Basics */
    wprintf(L"--- Wide Strings ---\n");
    wchar_t greeting[] = L"Héllo wörld! π Ω";
    wprintf(L"Greeting: %ls\n", greeting);
    
    size_t len = wcslen(greeting);
    size_t size = sizeof(greeting);
    wprintf(L"wcslen = %zu characters\n", len);
    wprintf(L"sizeof = %zu bytes (each wchar_t = %zu bytes on this platform)\n\n", size, sizeof(wchar_t));

    /* 3. Wide Character Classification */
    wprintf(L"--- Wide Character Classification ---\n");
    wprintf(L"iswalpha(L'Ω') = %ls (alphabetic)\n", iswalpha(L'Ω') ? L"YES" : L"NO");
    wprintf(L"iswdigit(L'5') = %ls (digit)\n", iswdigit(L'5') ? L"YES" : L"NO");
    wprintf(L"iswspace(L' ') = %ls (whitespace)\n\n", iswspace(L' ') ? L"YES" : L"NO");

    /* 4. The Portability Trap */
    wprintf(L"--- Portability Trap ---\n");
    wprintf(L"sizeof(wchar_t) = %zu bytes\n", sizeof(wchar_t));
    /*
     * Portability Note:
     * On Linux/macOS, wchar_t is 4 bytes (UTF-32).
     * On Windows, wchar_t is 2 bytes (UTF-16).
     * Sending wchar_t arrays over a network socket breaks cross-platform 
     * compatibility. UTF-8 (char array) is the industry standard for exchange.
     */
    wprintf(L"Note: On Windows this would be 2 bytes (UTF-16). Use UTF-8 for network/file exchange.\n\n");

    /* 5. Wide String Operations */
    wprintf(L"--- Wide String Operations ---\n");
    wchar_t dest_buf[64];
    
    if (safe_wcscpy(dest_buf, 64, greeting))
    {
        wprintf(L"wcscpy: %ls\n", dest_buf);
    }

    if (safe_wcscat(dest_buf, 64, L" - Status OK"))
    {
        wprintf(L"wcscat: %ls\n", dest_buf);
    }

    wprintf(L"wcscmp(\"ABC\", \"ABC\") = %d (equal)\n", wcscmp(L"ABC", L"ABC"));
    wprintf(L"wcscmp(\"ABC\", \"DEF\") < 0 (less than)\n");

    return 0;
}