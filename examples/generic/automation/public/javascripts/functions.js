function toogleTv(id) {
  $.get('toggleTv/' + id).success(
      function(data) {
	alert('Sent!');
      });
}

function getStatus(id, place) {
  $.get('getStatus/'+ id).success(function(data) {
    $('#'+ place + ' span').text(data);
  });
}

$(document).ready(function() {
  getStatus('room1', 'tv-status');

  window.setInterval(function() {
    getStatus('room1', 'tv-status');
  }, 5000);
});

