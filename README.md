# 00.Well_washing
Upwork_task
There will be UPTO 12 wells .
So, let us say the number of wells is n.
The last well that is the nth we’ll will be the drying well. It has a heater which may be switched ON during the drying step.
The last but one well, that is the (n-1)th well will be the washing well. There are two pumps connected to this well- one to fill washing solution and the other to drain the washing solution.

Sẽ có UPTO 12 giếng . Vì vậy, chúng ta hãy nói số lượng giếng là n. Giếng cuối cùng là giếng thứ n chúng ta sẽ là giếng sấy khô. Nó có một lò sưởi có thể được BẬT trong bước sấy. Cái cuối cùng nhưng một cái giếng, đó là giếng (n-1) sẽ là giếng rửa. Có hai máy bơm được kết nối với giếng này - một để đổ đầy dung dịch giặt và máy còn lại để xả dung dịch giặt.

The rest of the wells (that is n-2 wells ) contain different solutions in them.
There is a sample holder on which the sample is mounted.
The sample is dipped into the wells, held there for a programmable time, then shaken up and down for a programmable time, then held again for a programmable time then raised up again for a programmable time.

Phần còn lại của giếng (nghĩa là giếng n-2 ) chứa các giải pháp khác nhau trong đó. Có một giá đỡ mẫu mà mẫu được gắn trên đó. Mẫu được nhúng vào giếng, được giữ ở đó trong một thời gian có thể lập trình được, sau đó lắc lên xuống trong một thời gian có thể lập trình được, sau đó được giữ lại trong một thời gian có thể lập trình được sau đó được nâng lên một lần nữa trong một thời gian có thể lập trình được.

It is then moved to another well and the whole process is repeated.
There are two stepper motors . Stepper motor’Z’ is used to move the sample up and down and motor ‘X’ is used to move the sample from left to right and right to left.. When ‘Z’ motor turns clockwise the sample moves down. When ‘X’ motor moves clockwise the sample moves from right to left. Both motors have ‘Home’ positions, which are sensed by opto interrupters . The Z motor Home is at the top and the X motor Home is at extreme left. Before moving the X motor, it should always be ensured that the Z motor is in Home position.

Sau đó, nó được chuyển đến một giếng khác và toàn bộ quá trình được lặp lại. Có hai động cơ bước . Động cơ bước 'Z' được sử dụng để di chuyển mẫu lên xuống và động cơ 'X' được sử dụng để di chuyển mẫu từ trái sang phải và phải sang trái. Khi động cơ 'Z' quay theo chiều kim đồng hồ, mẫu sẽ di chuyển xuống. Khi động cơ 'X' di chuyển theo chiều kim đồng hồ, mẫu di chuyển từ phải sang trái. Cả hai động cơ đều có vị trí 'Home', được cảm nhận bởi bộ ngắt quang . Động cơ Z Home ở trên cùng và động cơ X Home ở cực bên trái. Trước khi di chuyển động cơ X, phải luôn đảm bảo rằng động cơ Z ở vị trí Home.

There should be a provision for storing ten independent programmes. Each programme can have upto 25 steps and each step will have 5 sub steps in ‘Z’ direction. A step is a movement of sample from one well to another (X direction) and sub step is movement in ‘Z’ direction. Please note that a sample can be moved from any well to any well and not necessarily from well 1 to well 2 to well 3 etc. .

Cần có một điều khoản để lưu trữ mười chương trình độc lập. Mỗi chương trình có thể có tối đa 25 bước và mỗi bước sẽ có 5 bước phụ theo hướng 'Z'. Một bước là một chuyển động của mẫu từ giếng này sang giếng khác (hướng X) và bước phụ là chuyển động theo hướng 'Z'. Xin lưu ý rằng một mẫu có thể được chuyển từ giếng bất kỳ sang giếng nào và không nhất thiết phải từ giếng 1 đến giếng 2 đến giếng 3, v.v. .

In the ‘Z’ direction there are following steps ( referred above as sub steps)
Wait at the top (Z Home ) for x1 seconds
Move to bottom for x2 seconds
Shake up/down at the bottom for x3 seconds
Wait again at the bottom for x4 seconds
Go to top (Home) for x5 seconds.
The above timing should be programmable for every well.
The washing well should be filled with solution before the start of every cycle and should be drained as per the programme.

Theo hướng 'Z' có các bước sau (được gọi ở trên là các bước phụ) Chờ ở trên cùng (Z Home) trong x1 giây Di chuyển xuống dưới cùng trong x2 giây Lắc lên / xuống ở dưới cùng trong x3 giây Chờ lại ở dưới cùng trong x4 giây Chuyển lên trên cùng (Trang chủ) trong x5 giây. Thời gian trên nên được lập trình cho mọi giếng. Giếng rửa phải được đổ đầy dung dịch trước khi bắt đầu mỗi chu kỳ và nên được xả theo chương trình.

The heater should be started only when the sample is lowered into the heating well and heater ON is chosen in the programme.
LCD is 2 line 16 character but if necessary you may use a 4 line LCD. Also, if possible, write the code so that IN FUTURE the LCD can be replaced by a GLCD.

Chỉ nên khởi động lò sưởi khi mẫu được hạ xuống giếng sưởi và BẬT lò sưởi được chọn trong chương trình. LCD có 2 dòng 16 ký tự nhưng nếu cần, bạn có thể sử dụng màn hình LCD 4 dòng. Ngoài ra, nếu có thể, hãy viết mã để TRONG TƯƠNG LAI màn hình LCD có thể được thay thế bằng GLCD.

A typical simple programme
On power ON Z motor goes to Home then X motor goes to Home position. Programme 1 is selected
Go to well 2 , wait 5 sec
Go to bottom position , wait 120 sec
Shake 24 sec. ( the sample will be moved up/dn through 15 millimetres )
Wait 10 sec

Một chương trình đơn giản điển hình Trên nguồn BẬT động cơ Z đi đến Trang chủ sau đó động cơ X đi đến vị trí Trang chủ. Chương trình 1 được chọn Đi đến giếng 2, đợi 5 giây Đi đến vị trí dưới cùng, đợi 120 giây Lắc 24 giây (mẫu sẽ được di chuyển lên / dn qua 15 mm) Đợi 10 giây

Go to Top, wait 5 sec
Go to well 4, wait 4 sec
Go to bottom and wait 100 sec
Shake for 0 sec
Wait for 0 sec
Go to Top
Wait for 2 sec
Go to wash
Wait 15 sec
Shake 15 sec
Wait 0 sec
Go to Top
Wait 2 sec
Go to drying
Switch heater ON
Wait 50 sec
Go to Top
END . ( The sample should go to ‘Z ‘Home and ‘X’ Home)
I have shown only four steps here ( well2 then well4 then wash well and lastly drying well. In reality there could be upto 25 such steps)
KẾT THÚC.
( Mẫu nên chuyển đến 'Z 'Home và 'X' Home) Tôi chỉ chỉ ra bốn bước ở đây (giếng2 sau đó giếng4 sau đó rửa sạch và cuối cùng là sấy khô. Trong thực tế có thể có tới 25 bước như vậy)


![image](https://user-images.githubusercontent.com/48339214/210906079-c0f3534f-d2da-41d2-84c1-a5480fbcef00.png)



