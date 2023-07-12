document.getElementById("btn").addEventListener("click", function(event) {
  
    //要素取得
    let tel = document.getElementById("tel").value;
    let pw = document.getElementById("pw").value;
  
    //POST
    let url = "http://localhost:10000";
  
    let requestBody = "?tel=" + tel + "&pw=" + pw;

    console.log(requestBody);
  
    let xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function(){
        if(xhr.readyState === 4){
            if(xhr.status === 301){
            }else if(xhr.status === 400){
                document.getElementById("result").innerHTML = xhr.responseText;
            }else{
                result.textContent = '問題が発生しました';
            }
        }else{
            result.textContent = '通信中...';
        }
    };
    xhr.open("POST", url, true);
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.send(requestBody);
});