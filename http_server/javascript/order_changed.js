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
});

document.getElementById('tab').addEventListener('change', function(){
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

//ボタンをクリックしたときGETでサーバへ送信
document.getElementById('btn').addEventListener('click', function(e){

    if(tmp_cnt == 1){
        window.alert('1商品以上入力して下さい');
    }else{
        if(!window.confirm('注文を確定')){
            e.preventDefault();
        }else{

            let url = "http://localhost:10000";

            let orders = document.getElementsByClassName("order");
            let cnt = 0;

            let params = [];
            for(let i = 0; i < orders.length; i++){
                let order = orders[i];
                let paramName = encodeURIComponent(order.name);
                let paramValue = encodeURIComponent(order.value);        
                
                cnt++;
                params.push(paramName + "=" + paramValue);
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

                xhr.open("GET", url, true);
                xhr.send();
            }else{
                document.getElementById("result").innerHTML = "注文可能な種類の数を超えています";
            }
        }
    }
    
});

let tmp_cnt = 1;
let checkboxes = document.querySelectorAll('.order-c');

function listener(){
    if(this.checked){
        if(tmp_cnt > 5){
            window.alert('5種類以上選択できません')
            this.checked = false;
        }else{
            let menu_name = this.parentNode.nextElementSibling.textContent;

            let tr = document.createElement('tr');
            let trid = document.createAttribute('id');
            trid.value = this.name;
            tr.setAttributeNode(trid);

            let td1 = document.createElement('td');
            let input_tag = document.createElement('input');
            td1.appendChild(input_tag);
            let input_type = document.createAttribute('type');
            input_type.value = "checkbox";
            input_tag.setAttributeNode(input_type);
            let input_class = document.createAttribute('class');
            input_class.value = "order-c-p";
            input_tag.setAttributeNode(input_class);
            let input_name = document.createAttribute('name');
            input_name.value = this.name;
            input_tag.setAttributeNode(input_name);
            let input_onClick = document.createAttribute('onclick');
            input_onClick.value = "cClickEvent(event)";
            input_tag.setAttributeNode(input_onClick);
            let input_default = document.createAttribute('checked');
            input_tag.setAttributeNode(input_default);

            let td2 = document.createElement('td');
            td2.textContent = menu_name;

            let td3 = document.createElement('td');

            let select = document.createElement('select');
            let cname = document.createAttribute('class');
            cname.value = "order";
            select.setAttributeNode(cname);
            let sname = document.createAttribute('name');
            sname.value = this.name;
            select.setAttributeNode(sname);

            let option1 = document.createElement('option');
            option1.textContent = 1;
            let ov1 = document.createAttribute('value');
            ov1.value = 1;
            option1.setAttributeNode(ov1);
            select.appendChild(option1);

            let option2 = document.createElement('option');
            option2.textContent = 2;
            let ov2 = document.createAttribute('value');
            ov2.value = 2;
            option2.setAttributeNode(ov2);
            select.appendChild(option2);

            let option3 = document.createElement('option');
            option3.textContent = 3;
            let ov3 = document.createAttribute('value');
            ov3.value = 3;
            option3.setAttributeNode(ov3);
            select.appendChild(option3);

            let option4 = document.createElement('option');
            option4.textContent = 4;
            let ov4 = document.createAttribute('value');
            ov4.value = 4;
            option4.setAttributeNode(ov4);
            select.appendChild(option4);

            let option5 = document.createElement('option');
            option5.textContent = 5;
            let ov5 = document.createAttribute('value');
            ov5.value = 5;
            option5.setAttributeNode(ov5);
            select.appendChild(option5);

            td3.appendChild(select);

            tr.appendChild(td1);
            tr.appendChild(td2);
            tr.appendChild(td3);

            let tmpBody = document.getElementById('tmp');

            tmpBody.appendChild(tr);

            tmp_cnt++;
        }

        
    }else{
        console.log(this.name);
        let node = document.getElementById(this.name);
        node.remove();
        tmp_cnt--;
    }
    console.log(tmp_cnt);
}

function pClickEvent(e){
    let checkboxes = document.querySelectorAll('.order-c');
    checkboxes.forEach((checkbox) => {
        checkbox.removeEventListener('change', listener, false);
        checkbox.addEventListener('change', listener, false);
    });
}

function cClickEvent(e){
    let deleteTarget = document.getElementById(e.target.name);
    deleteTarget.remove();
    tmp_cnt--;

    let changeCheckbox = document.getElementById(e.target.name + '-t');
    console.log(changeCheckbox);
    if(changeCheckbox.checked){
        changeCheckbox.checked = false;
    }
}