#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUMS 5
#define TOTAL_SEATS 10 

typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

BookingRequest requests[5] = {
    {1, "Nguyen Van An",  2},
    {2, "Tran Thi Bich",  1},
    {3, "Le Van Cuong",   3},
    {4, "Pham Thi Dung",  1},
    {5, "Hoang Van Em",   2}
};

pthread_t threads[THREAD_NUMS];
int seats_available = TOTAL_SEATS;
int cnt_fail = 0;
pthread_mutex_t lock;

static const char *seat_word(int n) {
    return (n == 1) ? "seat " : "seats";
}

// Hàm mà thread sẽ thực thi
void *book_ticket(void *arg) {
    BookingRequest* booking_info = (BookingRequest*)arg;

    printf("[Agent %d | TID %lu] Booking %d %s for %s...\n",
           booking_info->agent_id, (unsigned long)pthread_self(),
           booking_info->seats_wanted, seat_word(booking_info->seats_wanted),
           booking_info->customer);

    sleep(1); // buộc các thread thực sự chạy song song trước khi vào critical section

    /*
     * Check (seats_available >= seats_wanted) và deduct (seats_available -= ...)
     * PHẢI nằm trong CÙNG một lần lock/unlock. Nếu tách thành 2 lần lock riêng:
     *   lock(); if (seats_available >= n) { ... } unlock();
     *   lock(); seats_available -= n; unlock();
     * thì giữa 2 lần lock đó, một thread khác có thể chen vào và trừ hết số ghế
     * còn lại. Đến khi thread quay lại để thực sự trừ ghế ở lần lock thứ 2, số
     * ghế đã bị thread khác lấy mất — seats_available có thể bị âm hoặc bị bán
     * vượt quá số ghế thực có (race condition), dù bước "check" trước đó đã
     * thấy đủ ghế. Vì vậy check-and-deduct phải là một thao tác atomic duy nhất.
     */
    pthread_mutex_lock(&lock);
    if (seats_available >= booking_info->seats_wanted) {
        seats_available -= booking_info->seats_wanted;
        printf("[Agent %d] CONFIRMED: %d %s for %s. Remaining: %d\n",
               booking_info->agent_id, booking_info->seats_wanted,
               seat_word(booking_info->seats_wanted), booking_info->customer,
               seats_available);
    } else {
        cnt_fail++;
        printf("[Agent %d] SOLD OUT: needs %d seats, only %d left — booking failed.\n",
               booking_info->agent_id, booking_info->seats_wanted, seats_available);
    }
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    pthread_mutex_init(&lock, NULL);

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (%d agents, %d seats)\n", THREAD_NUMS, TOTAL_SEATS);
    printf("==============================================\n");

    for (int i = 0; i < THREAD_NUMS; i++) {
        pthread_t thread_id; // Biến lưu ID của thread
        if (pthread_create(&thread_id, NULL, book_ticket, &requests[i]) != 0) {
            perror("Lỗi không thể tạo thread");
            exit(1);
        }
        threads[i] = thread_id;
    }

    // Đợi thread kết thúc trước khi tiến trình chính thoát
    for (int i = 0; i < THREAD_NUMS; i++) {
        pthread_join(threads[i], NULL);
    }

    int seats_sold = TOTAL_SEATS - seats_available;
    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : %d\n", TOTAL_SEATS);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", cnt_fail);
    printf("=========================================\n");

    pthread_mutex_destroy(&lock);

    return 0;
}
