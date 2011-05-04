var options = {

    options: {
        'tooltips': false,    // boolean for check boxes
        //'group' : { 'option1': true, 'option2': false } // radio buttons
    },

    callbacks: {
        'tooltips': function () {
                        if (this.options.tooltips) {
                            $('.tip.click').tipTip({ activation: 'click', 
                                                     delay: 100,
                                                     fadeIn: 100,
                                                     fadeOut: 400,
                                                     edgeOffset: -250, 
                                                     edgeInset: 100, 
                                                     defaultPosition: 'top', 
                                                     position: 'absolute',
                                                     activeMethod: board.info.tooltip_active_method
                            });
                        } else {
                            $('.tip.click').tipTip('unbind');
                        }
                    },
        //'group': function () {
                    //console.log(this.options.group);
                //},
    },

    panel_selector : '#options',

    reset : function () {
        $(this.panel_selector).html('');
    },

    init: function () {
        if ($(this.panel_selector).children().length === 0) {
            $('canvas#board').attr({'class': 'tip click', 'title': 'Game board'});
            this.init_controls();
            this.init_event_bindings();
        }
    },

    init_event_bindings: function () {
        var that = this,
            opt,
            val,
            callback,
            input;

        function event_method (val) {
            var f = function () {};
            switch (typeof val) {
                case 'object':
                    f = radio;
                    break;
                case 'boolean':
                    f = checkbox;
                    break;
            }
            return f;
        };

        function checkbox (opt) {
            return function (event) {
                var input = $(this),
                    callback_args = [],
                    checked = input.attr('checked');
                that.options[opt] = !!checked;
                that.callbacks[opt].apply(that, callback_args);
            };
        };

        function radio (opt) {
            return function (event) {
                var inputs = $(this),
                    checked_value = inputs.filter(':checked').attr('id'),
                    callback_args = [checked_value],
                    option;
                for (option in that.options[opt]) {
                    if (!that.options[opt].hasOwnProperty(option)) continue;
                    if (option == checked_value) {
                        that.options[opt][option] = true;
                    } else {
                        that.options[opt][option] = false;
                    }
                }
                that.callbacks[opt].apply(that, callback_args);
            };
        };

        for (opt in this.options) {
            if (!this.options.hasOwnProperty(opt)) continue;
            val = this.options[opt];
            input = $('input[name="'+opt+'"]');
            callback = event_method(val)(opt);
            input.bind('change.options', callback)
            this.callbacks[opt].call(this);
        }
    },

    init_controls: function () { // interface_options in control_panel
        var container = $(this.panel_selector),
            table = $('<table></table>'),
            opt,
            val,
            grp,
            grp_val,
            tr,
            td,
            label,
            input;

        function get_opt_attrs(val, val2) {
            var attrs = {},
                type = 'hidden';
            switch (typeof val) {
                case 'object':
                    type = 'radio';
                    attrs.checked = val2 || '';
                    break;
                case 'boolean':
                    type = 'checkbox';
                    attrs.checked = val || '';
                    break;
            }
            attrs.type = type;
            return attrs;
        };

        for (opt in this.options) {
            if (!this.options.hasOwnProperty(opt)) continue;
            val = this.options[opt];
            if (typeof val === 'object') {
                for (grp in val) {
                    if (!val.hasOwnProperty(grp)) continue;
                    grp_val = val[grp];
                    tr = $('<tr></tr>');
                    td = $('<td></td>');
                    label = $('<label></label>').attr('for', grp)
                                                .html(grp);
                    td.append(label);
                    tr.append(td);
                    td = $('<td></td>');
                    input = $('<input></input>').attr($.extend({ 'name': opt,
                                                                 'id'  : grp }, get_opt_attrs(val, grp_val)));
                    td.append(input);
                    tr.append(td);
                    table.append(tr);
                }
                
            } else if (typeof val === 'boolean') {
                tr = $('<tr></tr>');
                td = $('<td></td>');
                label = $('<label></label>').attr('for', opt)
                                            .html(opt);
                td.append(label);
                tr.append(td);
                td = $('<td></td>');
                input = $('<input></input>').attr($.extend({ 'name': opt,
                                                             'id'  : opt }, get_opt_attrs(val)));
                td.append(input);
                tr.append(td);
                table.append(tr);
            }
            table.append($('<tr></tr>').append($('<td></td>').append('<hr>')));
        }

        container.append($('<hr>'));
        container.append(table);
    }
};
