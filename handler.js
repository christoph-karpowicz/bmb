function sendMessage() {
    const messageInput = $('#messageInput').val();

    $.ajax({
        url: "http://localhost:8000/producer.php",
        type: "POST",
        crossDomain: true,
        data: {
            message : messageInput 
        }
    })
    .done(function(response) {
        console.log(response);
    })
    .fail(function(error) {
        console.log(error);
    })
    .always(function() {
        console.log("complete");
    });

}