document.getElementById('btn').addEventListener('click', function(){

    let url = "http://localhost:10000";

    let orders = document.getElementsByClassName("order");
    let cnt = 0;

    let params = [];
    for(let i = 0; i < orders.length; i++){
        let order = orders[i];
        let paramName = encodeURIComponent(order.name);
        let paramValue = encodeURIComponent(order.value);
        
        if(paramValue != 0){
            cnt++;
            params.push(paramName + "=" + paramValue);
        }
    }
    params.unshift("order=" + cnt);
    let requestBody = params.join("&");

    console.log(requestBody);

    if(cnt <= 5){
        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function(){
            if(xhr.readyState === 4){
                if(xhr.status === 200){
                    document.getElementById("result").innerHTML = xhr.responseText;
                }else if(xhr.status === 400){
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
    }else{
        document.getElementById("result").innerHTML = "注文可能な種類の数を超えています";
    }
});