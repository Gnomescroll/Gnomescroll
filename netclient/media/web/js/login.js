$(document).ready(function()
{

// set saved credentials
$('input#username_or_email').val(Gnomescroll.gs_username);
$('input#password').val(Gnomescroll.gs_pass);

function successful_login(data)
{   // record parsed json object
    Gnomescroll.clear_message();
    if (data['errors'] !== undefined && data['errors']['count'] !== 0)
    {   // update html with errors
        gs_append_form_errors(data['errors'], 'Login');
    }
    else if (data['success'])
        gs_extract_and_set_token(data);
    else
        gs_auth_server_error();
}

$('form#login').submit(function(e)
{
    e.preventDefault();
    
    Gnomescroll.set_message("Authorizing...");

    var form = $(this);
    
    // clear previous errors displayed on form
    gs_clear_form_errors();

    // make ajax GET request to gnomescroll login page
    // extract the csrf token on response
    $.ajax({
        type: 'GET',
        url: Gnomescroll.login_url,
        success: function (data)
        {   // 3 results can happen for a login GET request
            // we can already be logged in, in which case we get the token
            // or we can get a csrf_token to use for logging in
            // or we can get NULL
            if (!data)
                gs_auth_server_error();
            else
            {
                var csrf_token = data['csrf_token'];
                if (csrf_token === undefined)   // we must be logged in already
                    successful_login(data);
                else
                {   // update local form with token and submit
                    $('input#csrf_token').val(csrf_token);
                    gs_submit_form(form, Gnomescroll.login_url, successful_login, gs_auth_server_error);
                }
            }
        },
        error: gs_auth_server_error,
        dataType: 'json',
        headers: {
            'X-Requested-With': 'XMLHttpRequest',
        },  
    });

    var username = $('input#username_or_email').val();
    Gnomescroll.save_username(username);
    var password = $('input#password').val();
    if (!$('input#remember_password').prop('checked'))
        password = '';  // will clear the password
    Gnomescroll.save_password(password);
    
    return false;
});

});
