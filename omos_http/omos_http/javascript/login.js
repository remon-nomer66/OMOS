document.getElementById("btn").addEventListener("click", function(event) {
    
    //POST
    let url = "http://localhost:10000"; //localhostは適宜開いているサーバに変更
    
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
    xhr.open("POST", url, true);
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.send('login=1&tel=' + encodeURIComponent(document.getElementById("tel").value) + '&pw=' + encodeURIComponent(document.getElementById("pw").value));
});
