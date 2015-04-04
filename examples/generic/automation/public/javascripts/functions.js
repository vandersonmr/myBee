function toogleTv(id) {
  $.get('toggleTv/' + id).success(
      function(data) {
        alert('Sent!');
      });
}
