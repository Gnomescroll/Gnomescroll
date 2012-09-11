$(document).ready(function()
{

function record_login_response(resp)
{   // error if evaluates to false
    var token = resp[Gnomescroll.token_name];
    if (token)
        Gnomescroll.set_token(token);
    else
        Gnomescroll.set_error("Authentication server failure.");
}

function make_error_element(msg)
{
    var el = $('<div class="alert alert-error"></div>');
    el.text(msg);
    return el;
}

function append_form_errors(errors)
{
    console.log(JSON.stringify(errors));
    $.each(errors, function(name, msgs)
    {
        var el = $('div#'+name+'.errors');
        var len = msgs.length;
        for (var i=0; i<len; i++)
            el.append(make_error_element(msgs[i]));
    });
}

function clear_form_errors()
{
    $('div.errors').each(function(i, el)
    {
        $(el).children().remove();
    });
}

$('form#login').submit(function(e)
{
    e.preventDefault();

    // clear previous errors displayed on form
    clear_form_errors();

    // grab form and form data
    var form = $(this);
    var data = form.serializeArray();

    var success = function(data, textStatus, jqXHR)
    {   // record parsed json object
        Gnomescroll.clear_error();
        if (data['errors'] !== undefined && data['errors']['count'] !== 0)
        {   // update html with errors
            append_form_errors(data['errors']);
        }
        else if (data['success'])
            record_login_response(data);
        else
            Gnomescroll.set_error("Authentication failed.");
    };

    var error = function(jqXHR, textStatus, errorThrown)
    {
        Gnomescroll.set_error("Authentication server failure.");
    };

    // make ajax POST request to gnomescroll login page
    // gnomescroll login page should return a json object containing errors or a gstoken
    $.ajax({
        type: 'POST',
        url: Gnomescroll.login_url,
        data: data,
        success: success,
        error: error,
        headers: {
            'X-Requested-With': 'XMLHttpRequest',
        },            
    });
    
    return false;
});

$('a#create').click(function(e)
{
    e.preventDefault();
    Gnomescroll.clear_error();

    var success = function()
    {
        window.location.href = Gnomescroll.create_url;        
    }

    var error = function()
    {
        Gnomescroll.set_error("Authentication server failure.");
    }

    $.ajax({
        type: 'GET',
        url: Gnomescroll.create_url,
        async: false,
        success: success,
        error: error,
        headers: {
            'X-Requested-With': 'XMLHttpRequest',
        },            
    });
    
    return false;
});

});
