document.addEventListener('DOMContentLoaded', function(){   //ロードされたときに実行

    //日付の制限
    let minDate = new Date();
    let maxDate = new Date();
    //console.log(minDate.toISOString().slice(11, 16));
    let reserveInputDate = document.getElementById('reservation-date');

    reserveInputDate.value = minDate.toISOString().slice(0, 10);
    maxDate.setMonth(maxDate.getMonth() + 1);
    reserveInputDate.min = minDate.toISOString().slice(0, 10);
    reserveInputDate.max = maxDate.toISOString().slice(0, 10);
    
    //1日後の日付
    let year = new Date().getFullYear();
    let month = new Date().getMonth() + 1;
    let date = new Date().getDate();

    if((month === 1 || month === 3 || month === 5 || month === 7 || month === 8 || month === 10 || month === 12) && date === 31){
        if(month === 12){
            year++;
            month = 1;
        }else{
            month++;
        }
        date = 1;
    }else if((month === 4 || month === 6 || month === 9 || month === 11) && date === 30){
        month++;
        date = 1;
    }else if(year % 4 === 0 && month === 2 && date === 29){
        month++;
        date = 1;
    }else if(month === 2 && date === 28){
        month++;
        date = 1;
    }else{
        date++;
    }
    if(month < 10){
        month = "0" + month;
    }
    if(date < 10){
        date = "0" + date;
    }

    let next = year + "-" + month + "-" + date;
    reserveInputDate.value = next;

    //時間の制限
    let reserveInputTime = document.getElementById('reservation-time');
    let selectOptionsTime = "";
    for(let i = 17; i <= 22; i++) {
        for(let j = 0; j < 4; j++) {
            let hour = i;
            let min = j * 15;

            if(min === 0){
                selectOptionsTime += '<option value="' + hour + ':0' + min + '">' + hour + ':0' + min + '</option>';
            }else{
                selectOptionsTime += '<option value="' + hour + ':' + min + '">' + hour + ':' + min + '</option>';
            }
        }
    }
    reserveInputTime.innerHTML = selectOptionsTime;

    //人数の制限
    let reserveInputNum = document.getElementById('reservation-person');
    let selectOptionsNum = "";
    for(let i = 1; i <= 10; i++) {
        selectOptionsNum += '<option value="' + i + '">' + i + '</option>';
    }
    reserveInputNum.innerHTML = selectOptionsNum;
});

document.getElementById('reservation-date').addEventListener('change', function(){

    let reserveInputDate = document.getElementById('reservation-date');
    let selectOptionsTime = "";
    let reserveInputTime = document.getElementById('reservation-time');

    if(reserveInputDate.value === new Date().toISOString().slice(0, 10)){
        console.log(reserveInputDate.value);
        for(let i = 17; i <= 22; i++) {
            for(let j = 0; j < 4; j++) {
                let time = new Date();
                if(time.getHours() > i){
                    break;
                }
                let hour = i;
                let min = j * 15;

                if(min === 0){
                    selectOptionsTime += '<option value="' + hour + ':0' + min + '">' + hour + ':0' + min + '</option>';
                }else{
                    selectOptionsTime += '<option value="' + hour + ':' + min + '">' + hour + ':' + min + '</option>';
                }
            }
        }
    }else{
        for(let i = 17; i <= 22; i++) {
            for(let j = 0; j < 4; j++) {
                let hour = i;
                let min = j * 15;
    
                if(min === 0){
                    selectOptionsTime += '<option value="' + hour + ':0' + min + '">' + hour + ':0' + min + '</option>';
                }else{
                    selectOptionsTime += '<option value="' + hour + ':' + min + '">' + hour + ':' + min + '</option>';
                }
            }
        }
    }
    reserveInputTime.innerHTML = selectOptionsTime;
});

document.addEventListener('DOMContentLoaded', function(){   //ロードされたときに実行
    let url = "http://localhost:10000?reserveCheck=1";

    let xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function(){
        if(xhr.readyState === 4){
            if(xhr.status === 200){
                document.getElementById("check").innerHTML = xhr.responseText;
            }else{
                check.textContent = '問題が発生しました';
            }
        }else{
            check.textContent = '通信中...';
        }
    };
    xhr.open("GET", url, false);
    xhr.send(null);
});

document.addEventListener('DOMContentLoaded', function(){   //ロードされたときに実行
    let url = "http://localhost:10000?storeInfo=1"; //localhostは適宜開いているサーバに変更

    let xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function(){
        if(xhr.readyState === 4){
            if(xhr.status === 200){
                document.getElementById("res").innerHTML = xhr.responseText;
            }else{
                res.textContent = '問題が発生しました';
            }
        }else{
            res.textContent = '通信中...';
        }
    };
    xhr.open("GET", url, true);
    xhr.send(null);
});

//ボタンをクリックしたときGETでサーバへ送信
function btnEvent(e){
    if(!window.confirm('予約を確定')){
        e.preventDefault();
    }else{
        let url = "http://localhost:10000?reserve=1&";  //localhostは適宜開いているサーバに変更

        let selectedSID = document.getElementById("reservation-store-id").value;
        let selectedDate = document.getElementById("reservation-date").value;
        let selectedTime = document.getElementById("reservation-time").value;
        let selectedPerson = document.getElementById("reservation-person").value;
        let result = document.getElementById('result');

        url += "storeId=";
        url += selectedSID;
        url += "&";

        url += "date=";
        url += selectedDate;
        url += "&";

        url += "time=";
        url += selectedTime;
        url += "&";

        url += "num=";
        url += selectedPerson;

        console.log(url);

        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function(){
            if(xhr.readyState === 4){
                if(xhr.status === 301){
                }else if(xhr.status === 200){
                    document.getElementById("result").innerHTML = xhr.responseText;
                }else{
                    result.textContent = '問題が発生しました';
                }
            }else{
                result.textContent = '通信中...';
            }
        };

        //falseで同期通信にしている
        xhr.open("GET", url, false);
        xhr.send();
    }
}
