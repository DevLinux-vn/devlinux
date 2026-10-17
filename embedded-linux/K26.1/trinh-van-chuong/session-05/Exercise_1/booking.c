/*
 * booking.c — Hệ thống mô phỏng đặt vé xe khách (5 đại lý, 10 ghế)
 *
 * Biên dịch bằng Makefile (hoặc lệnh: gcc -Wall -pthread -o booking booking.c)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

/* Khởi tạo dữ liệu cứng theo yêu cầu đề bài */
BookingRequest requests[5] = {
    {1, "Nguyen Van An",  2},
    {2, "Tran Thi Bich",  1},
    {3, "Le Van Cuong",   3},
    {4, "Pham Thi Dung",  1},
    {5, "Hoang Van Em",   2}
};

int seats_available = 10;
pthread_mutex_t seat_lock;

/* Các biến toàn cục phục vụ cho bảng tổng kết (Summary) */
int total_seats = 10;
int seats_sold = 0;
int failed_bookings = 0;

/* Hàm thực thi của từng Đại lý (Thread) */
void* book_ticket(void* arg) {
    BookingRequest* req = (BookingRequest*)arg;
    
    // In thông báo khi đại lý tiếp nhận yêu cầu (Trước khi vào Critical Section)
    printf("[Agent %d | TID %lu] Booking %d %s for %s...\n", 
           req->agent_id, 
           (unsigned long)pthread_self(), 
           req->seats_wanted, 
           req->seats_wanted > 1 ? "seats" : "seat", 
           req->customer);

    // Ép xảy ra hiện tượng đồng thời thực tế (Real Concurrency)
    sleep(1); 

    /* ============================================================
     * CRITICAL SECTION — VÙNG NGUY HIỂM ĐƯỢC BẢO VỆ
     * ============================================================ */
    pthread_mutex_lock(&seat_lock);

    if (seats_available >= req->seats_wanted) {
        // Đủ ghế: Thực hiện trừ ghế và xác nhận thành công
        seats_available -= req->seats_wanted;
        seats_sold += req->seats_wanted;
        printf("[Agent %d] CONFIRMED: %d %s for %s. Remaining: %d\n",
               req->agent_id,
               req->seats_wanted,
               req->seats_wanted > 1 ? "seats" : "seat",
               req->customer,
               seats_available);
    } else {
        // Không đủ ghế: Thông báo hết vé và tính vào đơn thất bại
        failed_bookings++;
        printf("[Agent %d] SOLD OUT:  needs %d seats, only %d left — booking failed.\n",
               req->agent_id,
               req->seats_wanted,
               seats_available);
    }

    pthread_mutex_unlock(&seat_lock);
    /* ============================================================
     * KẾT THÚC CRITICAL SECTION
     * ============================================================ */

    return NULL;
}

int main(void) {
    pthread_t threads[5];
    int i;

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    // 1. Khởi tạo ổ khóa Mutex
    if (pthread_mutex_init(&seat_lock, NULL) != 0) {
        perror("Không thể khởi tạo Mutex");
        return 1;
    }

    // 2. Tạo 5 luồng đại lý chạy song song
    for (i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, book_ticket, &requests[i]);
    }

    // Đoạn text phân tách dòng thời gian giống yêu cầu đề bài hiển thị
    // Chờ 0.5s để đống printf tiếp nhận đơn in ra trước, tạo cảm giác mạch lạc
    usleep(500000);
    printf("\n--- [all agents reach critical section after sleep(1)] ---\n\n");

    // 3. Chờ cả 5 luồng chạy xong để dọn dẹp xác
    for (i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    // 4. Hủy ổ khóa Mutex
    pthread_mutex_destroy(&seat_lock);

    // 5. In bảng tổng kết cuối cùng
    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : %d\n", total_seats);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    return 0;
}

/* ============================================================================
 * COMMENT BLOCK: GIẢI THÍCH TẠI SAO PHẢI LOCK CHUNG KHỐI (CHECK & DEDUCT)
 * * Bản chất của thao tác đặt vé an toàn gồm hai bước logic:
 * Bước A (Check): Nhìn vào kho xem còn đủ ghế trống hay không.
 * Bước B (Deduct): Nếu đủ thì tiến hành trừ số ghế trống đi.
 * * BẮT BUỘC phải đặt Bước A và Bước B vào trong CÙNG MỘT khối lock/unlock vì:
 * * Nếu chúng ta tách rời thành hai lần khóa riêng biệt như thế này:
 * pthread_mutex_lock(&seat_lock);
 * bool ok = (seats_available >= wanted);  // Chỉ khóa để check
 * pthread_mutex_unlock(&seat_lock);
 * * if (ok) {
 * pthread_mutex_lock(&seat_lock);
 * seats_available -= wanted;          // Khóa lại để trừ ghế
 * pthread_mutex_unlock(&seat_lock);
 * }
 * * Kịch bản lỗi kinh điển sẽ lập tức xảy ra khi chạy đa luồng (Race Condition):
 * 1. Giả sử chuyến xe chỉ còn đúng 2 ghế trống.
 * 2. Đại lý 1 (cần 2 ghế) khóa cửa và check -> thấy còn 2 ghế (Hợp lệ!). 
 * Sau đó Đại lý 1 nhả khóa ra để chuẩn bị thực hiện lệnh trừ.
 * 3. Ngay tích tắc Đại lý 1 nhả khóa (chưa kịp trừ ghế), Hệ điều hành ngắt luồng 1 
 * để nhường CPU cho Đại lý 5 (cũng cần 2 ghế).
 * 4. Đại lý 5 khóa cửa và check -> vì Đại lý 1 chưa trừ, số ghế vẫn đang là 2. 
 * Đại lý 5 thấy hợp lệ, liền trừ luôn 2 ghế thành 0 rồi mở khóa đi ra.
 * 5. Hệ điều hành cho Đại lý 1 chạy tiếp. Vì biến 'ok' của Đại lý 1 đã được tính 
 * từ trước là "Hợp lệ", Đại lý 1 lao vào khóa cửa và trừ tiếp 2 ghế:
 * seats_available = 0 - 2 = -2 ghế! (Xe bị bán lố, âm chỗ).
 * * Kết luận: Việc tách ổ khóa sẽ tạo ra một "khoảng trống thời gian" giữa lúc check 
 * và lúc trừ. Biến điều kiện hoàn toàn có thể bị thay đổi bởi luồng khác trong 
 * khoảng trống đó. Gom chung vào 1 khóa giúp hành động Check-and-Deduct trở thành 
 * một khối nguyên tử (Atomic), bất khả xâm phạm.
 * ============================================================================ */