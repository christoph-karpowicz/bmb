const URL = "http://localhost:8080";

// Producer
function produceMessage() {

    const messageInput = $('#messageInput').val();

    $.ajax({
        url: URL,
        type: "POST",
        crossDomain: true,
        dataType:"json",
        data: {
            type: "produce",
            queue: 0,
            message : messageInput
        }
    })
    .done(function(response) {
        console.log(response);
    })
    .fail(function(error, textStatus, errorThrown) {
        console.log(error);
        console.log(textStatus);
        console.log(errorThrown);
    });

}

$("form#producer").submit((e) => {

    e.preventDefault();
    produceMessage();

});

// Consumer
function consumeMessage() {

    const output = $('#output');

    $.ajax({
        url: URL,
        type: "GET",
        crossDomain: true,
        dataType:"json",
        data: {
            type: "consume",
            queue : 0
        }
    })
    .done(function(response) {
        console.log(response);
        $(output).html(response.msg);
    })
    .fail(function(error) {
        console.log(error);
    });    

}

$("form#consumer").submit((e) => {

    e.preventDefault();
    consumeMessage();

});