const myButton = document.getElementById('myButton');

myButton.addEventListener('click', function() {
  // 確認ダイアログを表示
  const result = confirm('本当に処理を実行しますか？');

  // OKボタンがクリックされた場合の処理
  if (result) {
    // 実際の処理をここに記述する
    console.log('処理を実行します');
  }
});