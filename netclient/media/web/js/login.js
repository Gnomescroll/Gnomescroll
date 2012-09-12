$(document).ready(function()
{

function record_login_response(resp)
{   // extract token from json response, and notify the game client
    var token = resp[Gnomescroll.token_name];
    if (token)
        Gnomescroll.set_token(token);
    else // error if token evaluates to false
        Gnomescroll.set_error("Authentication server failure.");
}

function make_error_element(msg)
{   // create an element for rendering errors
    var el = $('<div class="alert alert-error"></div>');
    el.text(msg);
    return el;
}

function append_form_errors(errors)
{   // attach errors received from json response to the form
    $.each(errors, function(name, msgs)
    {
        var el = $('div#'+name+'.errors');
        var len = msgs.length;
        for (var i=0; i<len; i++)
            el.append(make_error_element(msgs[i]));
    });
}

function clear_form_errors()
{   // remove html error indicators
    $('div.errors').each(function(i, el)
    {
        $(el).children().remove();
    });
}

function submit_login(form)
{
    // grab form and form data
    var data = form.serializeArray();

    var success = function(data, text_status, jqXHR)
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

    var error = function(jqXHR, text_status, error_thrown)
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
        dataType: 'json',
    });
}

$('form#login').submit(function(e)
{
    e.preventDefault();

    var form = $(this);
    
    // clear previous errors displayed on form
    clear_form_errors();

    // make ajax GET request to gnomescroll login page
    // extract the csrf token on response
    $.ajax({
        type: 'GET',
        url: Gnomescroll.login_url,
        success: function (data)
        {
            // extract csrf token from returned form
            var csrf_token = $('input#csrf_token', data).val();
            // update local form with token
            $('input#csrf_token').val(csrf_token);
            // submit the form
            submit_login(form);
        },
        error: function (jqXHR, text_status, error_thrown)
        {
            Gnomescroll.set_error("Authentication server failure.");
        },
        dataType: 'html',
        headers: {
            'X-Requested-With': 'XMLHttpRequest',
        },  
    });

    return false;
});

$('a#create').click(function(e)
{   // navigating to create page
    e.preventDefault();

    // clear errors set previously
    Gnomescroll.clear_error();

    var success = function()
    {   // if the page loads, set it as the url
        window.location.href = Gnomescroll.create_url;        
    }

    var error = function()
    {   // notify system
        Gnomescroll.set_error("Authentication server failure.");
    }

    // make a GET request to the create page
    // if it fails, notify user of error
    // if it succeeds, navigate to the page
    // we must do this because awesomium does not have any way of detecting
    // a server that is not available
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
