var options = {

    options: {
        'tooltips': true,    // boolean for check boxes
        //'group' : { 'option1': true, 'option2': false } // radio buttons (not fully implemented)
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
                             activeMethod: board.info.tooltip_active_method,
                            });
                        } else {
                            $('.tip.click').tipTip('unbind');
                        }
                    },
    },

    init: function () {
        this.init_controls();
        this.init_event_bindings();
    },

    init_event_bindings: function () {
        var that = this,
            opt,
            val;

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
            };
        };

        for (opt in this.options) {
            if (!this.options.hasOwnProperty(opt)) continue;
            val = this.options[opt];
            $('input[name="'+opt+'"]').change(event_method(val)(opt))
                                      .trigger('change');
        }
    },

    init_controls: function () { // interface_options in control_panel
        var container = $('#options'),
            table = $('<table></table>'),
            opt,
            val,
            tr,
            td,
            label,
            input;

        function get_opt_attrs(val) {
            var attrs = {},
                type = 'hidden';
            switch (typeof val) {
                case 'object':
                    type = 'radio';
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
            tr = $('<tr></tr>');
            td = $('<td></td>');
            label = $('<label></label>').attr('for', opt)
                                        .html(opt);
            td.append(label);
            tr.append(td);
            td = $('<td></td>');
            input = $('<input></input>').attr('name', opt)
                                        .attr(get_opt_attrs(val));
            td.append(input);
            tr.append(td);
            table.append(tr);
        }

        container.append($('<hr>'));
        container.append(table);
    }
};
