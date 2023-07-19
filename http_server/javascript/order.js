/* document.addEventListener('DOMContentLoaded', function(){
    let url = "http://133.54.224.159:10000?orderCheck=1&layer=1";

    let xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function(){
        if(xhr.readyState === 4){
            if(xhr.status === 200){
                document.getElementById("prev").innerHTML = xhr.responseText;
            }else{
                prev.textContent = '問題が発生しました';
            }
        }else{
            prev.textContent = '通信中...';
        }
    }
    xhr.open("GET", url, true);
    xhr.send(null);
}); */

/* document.getElementById('tab').addEventListener('change', function(){
    let url = "http://133.54.224.159:10000?orderCheck=1&";
    url += "layer=";
    url += encodeURIComponent(document.getElementById("tab").value);

    let xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function(){
        if(xhr.readyState === 4){
            if(xhr.status === 200){
                document.getElementById("prev").innerHTML = xhr.responseText;
            }else{
                prev.textContent = '問題が発生しました';
            }
        }else{
            res.textContent = '通信中...';
        }
    }
    xhr.open("GET", url, true);
    xhr.send(null);
}); */

const checkMax = 3;
const checkBoxes = document.getElementsByName('order-c');

function checkCount(target) {
  let checkCount = 0;
  checkBoxes.forEach(checkBox => {
    if (checkBox.checked) {
      checkCount++;
    }
  });
  if (checkCount > checkMax) {
    alert('最大3つまで');
    target.checked = false;
  }
}

checkBoxes.forEach(checkBox => {
  checkBox.addEventListener('change', () => {
    checkCount(checkBox);
  })
});

//ボタンをクリックしたときGETでサーバへ送信
document.getElementById('btn').addEventListener('click', function(){

    let url = "http://localhost:10000";

    let orders = document.getElementsByClassName("order");
    let cnt = 0;

    let params = [];
    for(let i = 0; i < orders.length; i++){
        let order = orders[i];
        let paramName = encodeURIComponent(order.name);
        let paramValue = encodeURIComponent(order.value);

        let tf = document.getElementById(paramName).checked;
        
        console.log(paramName);
        console.log(tf);
        
        if(paramValue != 0 && tf){
            cnt++;
            params.push(paramName + "=" + paramValue);
        }
    }
    params.unshift("?order=" + cnt);
    let requestBody = params.join("&");
    url += requestBody;

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

        console.log(url);

        //falseで同期通信にしている
        xhr.open("GET", url, false);
        xhr.send();
    }else{
        document.getElementById("result").innerHTML = "注文可能な種類の数を超えています";
    }
});