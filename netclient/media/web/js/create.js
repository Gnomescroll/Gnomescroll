$(document).ready(function()
{

function successful_create(data)
{   // record parsed json object
    Gnomescroll.clear_message();
    if (data['errors'] !== undefined && data['errors']['count'] !== 0)
    {   // update html with errors
        gs_append_form_errors(data['errors'], 'Create');
    }
    else if (data['success'])
        gs_extract_and_set_token(data);
    else
        gs_auth_server_error();
    if (data['messages'] !== undefined)
    {
        gs_append_form_messages(data['messages']);
    }
}

$('form#create').submit(function(e)
{
    e.preventDefault();

    var form = $(this);

    Gnomescroll.set_message("Creating account...");

    // clear previous errors displayed on form
    gs_clear_form_errors();

    // make ajax GET request to gnomescroll create page
    // extract the csrf token on response
    $.ajax({
        type: 'GET',
        url: Gnomescroll.create_url,
        success: function (data)
        {   // 2 results can happen for a create GET request
            // we can already be logged in, in which case we get the token
            // or we can get a csrf_token to use for creating
            var csrf_token = data['csrf_token'];
            if (csrf_token === undefined)   // we must be logged in already
            {   // set the token
                gs_extract_and_set_token(data);
            }
            else
            {   // update local form with token and submit
                $('input#csrf_token').val(csrf_token);
                gs_submit_form(form, Gnomescroll.create_url, successful_create, gs_auth_server_error);
            }
        },
        error: gs_auth_server_error,
        dataType: 'json',
        headers: {
            'X-Requested-With': 'XMLHttpRequest',
        },
    });

    return false;
});

});
