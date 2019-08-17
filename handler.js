function produceMessage() {

    const messageInput = $('#messageInput').val();

    $.ajax({
        url: "http://localhost:8080",
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

function consumeMessage() {

    const output = $('#output');

    $.ajax({
        url: "http://localhost:8080",
        type: "GET",
        crossDomain: true,
        data: {
            queue : 0 
        }
    })
    .done(function(response) {
        console.log(response);
        $(output).html(JSON.stringify(response));
    })
    .fail(function(error) {
        console.log(error);
    })
    .always(function() {
        console.log("complete");
    });
    
}