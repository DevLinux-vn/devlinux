#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define TRANSACTION_COUNT   5   /* so vong lap giao dich */
#define SAFE_SECTION_SEC    3   /* thoi gian gia lap ghi log (khong duoc ngat) */
#define IDLE_SECTION_SEC    3   /* thoi gian nghi (co the bi ngat binh thuong) */

int main(void)
{
    sigset_t block_set; /* tap hop signal can block: chi co SIGINT */
    sigset_t old_set;   /* luu lai mask hien tai TRUOC khi minh block */

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    for (int i = 1; i <= TRANSACTION_COUNT; i++) {

        /* ================= SAFE (critical section) ================= */
        /* Block SIGINT, đồng thời lưu mask cũ vào old_set qua tham số
         * thứ 3. Nếu Ctrl+C được gửi trong lúc này, kernel sẽ giữ
         * SIGINT ở trạng thái pending thay vì xử lý ngay. */
        if (sigprocmask(SIG_BLOCK, &block_set, &old_set) != 0) {
            perror("sigprocmask(SIG_BLOCK) failed");
            return EXIT_FAILURE;
        }

        printf("[SAFE] Writing transaction #%d ...\n", i);
        fflush(stdout);

        sleep(SAFE_SECTION_SEC);

        printf("[SAFE] Transaction #%d committed.\n", i);
        fflush(stdout);

        /* Khoi phuc DUNG trang thai mask truoc do (SIG_SETMASK + old_set),
         * thay vi goi sigprocmask(SIG_UNBLOCK, &block_set, NULL).
         *
         * Ly do: SIG_UNBLOCK "cung" se luon mo khoa SIGINT, bat ke truoc
         * do no co dang bi block boi mot ly do khac (vi du: tien trinh
         * cha da chu dong block SIGINT truoc khi goi chuong trinh nay)
         * hay khong. Dieu do vi pham nguyen tac "khong tu y pha trang
         * thai minh khong tao ra". Dung old_set moi dam bao ta chi
         * hoan tac chinh xac phan thay doi cua minh, giu nguyen moi thu
         * khac nhu truoc khi ta can thiep. */
        if (sigprocmask(SIG_SETMASK, &old_set, NULL) != 0) {
            perror("sigprocmask(restore) failed");
            return EXIT_FAILURE;
        }

        /* ================= IDLE ================= */
        /* SIGINT khong bi block o day -> Ctrl+C se duoc xu ly theo
         * default action (ket thuc chuong trinh ngay), vi ta khong
         * dang ky handler rieng cho SIGINT trong bai nay. */
        printf("[IDLE] Waiting for next transaction...\n");
        fflush(stdout);

        sleep(IDLE_SECTION_SEC);
    }

    printf("[INFO] All transactions processed, exiting normally.\n");
    return EXIT_SUCCESS;
}