# use for filtering properties for json dicts
def filter_props(obj, properties):
    ptype = type(properties)
    if not (ptype == list or ptype == tuple):
        properties = [properties]
    d = {}
    for prop in properties:
        val = getattr(obj, prop, None)
        if val is None:
            continue
        if type(val) == bool:
            val = int(val)
        d[prop] = val
    return d
