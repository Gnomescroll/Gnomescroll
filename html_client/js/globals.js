var globals = {

    _generate_id : function () {    // generate random 16 char string
        var chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXTZabcdefghiklmnopqrstuvwxyz",
            num_chars = chars.length,
            string_length = 16,
            randomstring = '',
            i = 0,
            rnum;
        for (i=0; i<string_length; i++) {
            rnum = Math.floor(Math.random() * num_chars);
            randomstring += chars.charAt(rnum);
        }
        return randomstring;
    },

    create_client_id : function () {
        this.client_id = this._generate_id();
        localStorage.client_id = this.client_id;
        this.new_client = true;
    },
    
    update : function (params) {
        this.world_id   = typeof parseInt(params.world_id, 10)  === 'number' ? params.world_id  : this.world_id;
        this.client_id  = typeof parseInt(params.client_id, 10) === 'number' ? params.client_id : this.client_id;
        this.server_out = typeof params.server_out !== undefined ? params.server_out : this.server_out;
    },
};
