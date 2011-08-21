var container, interval,
camera, scene, renderer,
projector, plane, cube, linesMaterial,
color = 0,colors = [ 0xDF1F1F, 0xDFAF1F, 0x80DF1F, 0x1FDF50, 0x1FDFDF, 0x1F4FDF, 0x7F1FDF, 0xDF1FAF, 0xEFEFEF, 0x303030 ],
ray, brush, objectHovered,
mouse3D, isMouseDown = false, onMouseDownPosition,
radious = 1600, theta = 45, onMouseDownTheta = 45, phi = 60, onMouseDownPhi = 60,
isShiftDown = false;

init();
render();

function init() {

    container = document.createElement( 'div' );
    document.body.appendChild( container );

    var info = document.createElement( 'div' );
    info.style.position = 'absolute';
    info.style.top = '5px';
    info.style.width = '100%';
    info.style.textAlign = 'center';
    info.innerHTML = '<span style="color: #444; background-color: #fff; border-bottom: 1px solid #ddd; padding: 8px 10px; text-transform: uppercase;"><strong>0 - 9</strong>: colors, <strong>click</strong>: add voxel, <strong>shift + click</strong>: remove voxel, <strong>drag</strong>: rotate | <a id="link" href="" target="_blank">share</a> <a href="javascript:save();">save</a> <a href="javascript:clear();">clear</a></span>';
    container.appendChild( info );

    camera = new THREE.Camera( 40, window.innerWidth / window.innerHeight, 1, 10000 );
    camera.position.x = radious * Math.sin( theta * Math.PI / 360 ) * Math.cos( phi * Math.PI / 360 );
    camera.position.y = radious * Math.sin( phi * Math.PI / 360 );
    camera.position.z = radious * Math.cos( theta * Math.PI / 360 ) * Math.cos( phi * Math.PI / 360 );
    camera.target.position.y = 200;

    scene = new THREE.Scene();

    // Grid

    var geometry = new THREE.Geometry();
    geometry.vertices.push( new THREE.Vertex( new THREE.Vector3( - 500, 0, 0 ) ) );
    geometry.vertices.push( new THREE.Vertex( new THREE.Vector3( 500, 0, 0 ) ) );

    linesMaterial = new THREE.LineColorMaterial( 0x000000, 0.2 );

    for ( var i = 0; i <= 20; i ++ ) {

        var line = new THREE.Line( geometry, linesMaterial );
        line.position.z = ( i * 50 ) - 500;
        scene.addObject( line );

        var line = new THREE.Line( geometry, linesMaterial );
        line.position.x = ( i * 50 ) - 500;
        line.rotation.y = 90 * Math.PI / 180;
        scene.addObject( line );

    }

    projector = new THREE.Projector();

    plane = new THREE.Mesh( new Plane( 1000, 1000 ) );
    plane.rotation.x = - 90 * Math.PI / 180;
    scene.addObject( plane );

    cube = new Cube( 50, 50, 50 );

    ray = new THREE.Ray( camera.position, null );

    brush = new THREE.Mesh( cube, new THREE.MeshColorFillMaterial( colors[ color ], 0.4 ) );
    brush.position.y = 2000;
    brush.overdraw = true;
    scene.addObject( brush );

    onMouseDownPosition = new THREE.Vector2();

    // Lights

    var ambientLight = new THREE.AmbientLight( 0x404040 );
    scene.addLight( ambientLight );

    var directionalLight = new THREE.DirectionalLight( 0xffffff );
    directionalLight.position.x = 1;
    directionalLight.position.y = 1;
    directionalLight.position.z = 0.75;
    directionalLight.position.normalize();
    scene.addLight( directionalLight );

    var directionalLight = new THREE.DirectionalLight( 0x808080 );
    directionalLight.position.x = - 1;
    directionalLight.position.y = 1;
    directionalLight.position.z = - 0.75;
    directionalLight.position.normalize();
    scene.addLight( directionalLight );

    renderer = new THREE.CanvasRenderer();
    renderer.setSize( window.innerWidth, window.innerHeight );

    container.appendChild(renderer.domElement);

    document.addEventListener( 'keydown', onDocumentKeyDown, false );
    document.addEventListener( 'keyup', onDocumentKeyUp, false );

    document.addEventListener( 'mousemove', onDocumentMouseMove, false );
    document.addEventListener( 'mousedown', onDocumentMouseDown, false );
    document.addEventListener( 'mouseup', onDocumentMouseUp, false );

    document.addEventListener( 'mousewheel', onDocumentMouseWheel, false );

    if ( window.location.hash ) {

        buildFromHash();

    }

}

function onDocumentKeyDown( event ) {

    switch( event.keyCode ) {

        case 49: setBrushColor( 0 ); break;
        case 50: setBrushColor( 1 ); break;
        case 51: setBrushColor( 2 ); break;
        case 52: setBrushColor( 3 ); break;
        case 53: setBrushColor( 4 ); break;
        case 54: setBrushColor( 5 ); break;
        case 55: setBrushColor( 6 ); break;
        case 56: setBrushColor( 7 ); break;
        case 57: setBrushColor( 8 ); break;
        case 48: setBrushColor( 9 ); break;

        case 16: isShiftDown = true; interact(); render(); break;

        case 37: offsetScene( - 1, 0 ); break;
        case 38: offsetScene( 0, - 1 ); break;
        case 39: offsetScene( 1, 0 ); break;
        case 40: offsetScene( 0, 1 ); break;

    }

}

function onDocumentKeyUp( event ) {

    switch( event.keyCode ) {

        case 16: isShiftDown = false; interact(); render(); break;

    }

}

function onDocumentMouseDown( event ) {

    event.preventDefault();

    isMouseDown = true;

    onMouseDownTheta = theta;
    onMouseDownPhi = phi;
    onMouseDownPosition.x = event.clientX;
    onMouseDownPosition.y = event.clientY;

}

function onDocumentMouseMove( event ) {

    event.preventDefault();

    if ( isMouseDown ) {

        theta = - ( ( event.clientX - onMouseDownPosition.x ) * 0.5 ) + onMouseDownTheta;
        phi = ( ( event.clientY - onMouseDownPosition.y ) * 0.5 ) + onMouseDownPhi;

        phi = Math.min( 180, Math.max( 0, phi ) );

        camera.position.x = radious * Math.sin( theta * Math.PI / 360 ) * Math.cos( phi * Math.PI / 360 );
        camera.position.y = radious * Math.sin( phi * Math.PI / 360 );
        camera.position.z = radious * Math.cos( theta * Math.PI / 360 ) * Math.cos( phi * Math.PI / 360 );
        camera.updateMatrix();

    }

    mouse3D = projector.unprojectVector( new THREE.Vector3( ( event.clientX / renderer.domElement.width ) * 2 - 1, - ( event.clientY / renderer.domElement.height ) * 2 + 1, 0.5 ), camera );
    ray.direction = mouse3D.subSelf( camera.position ).normalize();

    interact();
    render();

}

function onDocumentMouseUp( event ) {

    event.preventDefault();

    isMouseDown = false;

    onMouseDownPosition.x = event.clientX - onMouseDownPosition.x;
    onMouseDownPosition.y = event.clientY - onMouseDownPosition.y;

    if ( onMouseDownPosition.length() > 5 ) {

        return;

    }

    var intersect, intersects = ray.intersectScene( scene );

    if ( intersects.length > 0 ) {

        intersect = intersects[ 0 ].object == brush ? intersects[ 1 ] : intersects[ 0 ];

        if ( intersect ) {

            if ( isShiftDown ) {

                if ( intersect.object != plane ) {

                    scene.removeObject( intersect.object );

                }

            } else {

                var position = new THREE.Vector3().add( intersect.point, intersect.object.matrixRotation.transform( intersect.face.normal.clone() ) );

                var voxel = new THREE.Mesh( cube, new THREE.MeshColorFillMaterial( colors[ color ] ) );
                voxel.position.x = Math.floor( position.x / 50 ) * 50 + 25;
                voxel.position.y = Math.floor( position.y / 50 ) * 50 + 25;
                voxel.position.z = Math.floor( position.z / 50 ) * 50 + 25;
                voxel.overdraw = true;
                scene.addObject( voxel );

            }

        }

    }

    updateHash();
    interact();
    render();

}

function onDocumentMouseWheel( event ) {

    radious -= event.wheelDeltaY;

    camera.position.x = radious * Math.sin( theta * Math.PI / 360 ) * Math.cos( phi * Math.PI / 360 );
    camera.position.y = radious * Math.sin( phi * Math.PI / 360 );
    camera.position.z = radious * Math.cos( theta * Math.PI / 360 ) * Math.cos( phi * Math.PI / 360 );
    camera.updateMatrix();

    interact();
    render();

}

function setBrushColor( value ) {

    color = value;
    brush.material[ 0 ].color.setHex( colors[ color ] ^ 0x4C000000 );

    render();

}

function buildFromHash() {

    var hash = window.location.hash.substr( 1 ),
    version = hash.substr( 0, 2 );

    if ( version == "A/" ) {

        var current = { x: 0, y: 0, z: 0, c: 0 }
        var data = decode( hash.substr( 2 ) );
        var i = 0, l = data.length;

        while ( i < l ) {

            var code = data[ i ++ ].toString( 2 );

            if ( code.charAt( 1 ) == "1" ) current.x += data[ i ++ ] - 32;
            if ( code.charAt( 2 ) == "1" ) current.y += data[ i ++ ] - 32;
            if ( code.charAt( 3 ) == "1" ) current.z += data[ i ++ ] - 32;
            if ( code.charAt( 4 ) == "1" ) current.c += data[ i ++ ] - 32;
            if ( code.charAt( 0 ) == "1" ) {

                var voxel = new THREE.Mesh( cube, new THREE.MeshColorFillMaterial( colors[ current.c ] ) );
                voxel.position.x = current.x * 50 + 25;
                voxel.position.y = current.y * 50 + 25;
                voxel.position.z = current.z * 50 + 25;
                voxel.overdraw = true;
                scene.addObject( voxel );

            }
        }

    } else {

        var data = decode( hash );

        for ( var i = 0; i < data.length; i += 4 ) {

            var voxel = new THREE.Mesh( cube, new THREE.MeshColorFillMaterial( colors[ data[ i + 3 ] ] ) );
            voxel.position.x = ( data[ i ] - 20 ) * 25;
            voxel.position.y = ( data[ i + 1 ] + 1 ) * 25;
            voxel.position.z = ( data[ i + 2 ] - 20 ) * 25;
            voxel.overdraw = true;
            scene.addObject( voxel );

        }

    }

    updateHash();

}

function updateHash() {

    var data = [],
    current = { x: 0, y: 0, z: 0, c: 0 },
    last = { x: 0, y: 0, z: 0, c: 0 },
    code;

    for ( var i in scene.objects ) {

        object = scene.objects[ i ];

        if ( object instanceof THREE.Mesh && object !== plane && object !== brush ) {

            current.x = ( object.position.x - 25 ) / 50;
            current.y = ( object.position.y - 25 ) / 50;
            current.z = ( object.position.z - 25 ) / 50;
            current.c = colors.indexOf( object.material[ 0 ].color.hex & 0xffffff );

            code = 0;

            if ( current.x != last.x ) code += 1000;
            if ( current.y != last.y ) code += 100;
            if ( current.z != last.z ) code += 10;
            if ( current.c != last.c ) code += 1;

            code += 10000;

            data.push( parseInt( code, 2 ) );

            if ( current.x != last.x ) {

                data.push( current.x - last.x + 32 );
                last.x = current.x;

            }

            if ( current.y != last.y ) {

                data.push( current.y - last.y + 32 );
                last.y = current.y;

            }

            if ( current.z != last.z ) {

                data.push( current.z - last.z + 32 );
                last.z = current.z;

            }

            if ( current.c != last.c ) {

                data.push( current.c - last.c + 32 );
                last.c = current.c;

            }

        }

    }

    data = encode( data );
    window.location.hash = "A/" + data;
    document.getElementById( 'link' ).href = "http://mrdoob.com/projects/voxels/#A/" + data;

}

function offsetScene( x, z ) {

    var offset = new THREE.Vector3( x, 0, z ).multiplyScalar( 50 );

    for ( var i in scene.objects ) {

        object = scene.objects[ i ];

        if ( object instanceof THREE.Mesh && object !== plane && object !== brush ) {

            object.position.addSelf( offset );

        }

    }

    updateHash();
    interact();
    render();

}

function interact() {

    if ( objectHovered ) {

        objectHovered.material[ 0 ].color.a = 1;
        objectHovered.material[ 0 ].color.updateStyleString();
        objectHovered = null;

    }

    var position, intersect, intersects = ray.intersectScene( scene );

    if ( intersects.length > 0 ) {

        intersect = intersects[ 0 ].object != brush ? intersects[ 0 ] : intersects[ 1 ];

        if ( intersect ) {

            if ( isShiftDown ) {

                if ( intersect.object != plane ) {

                    objectHovered = intersect.object;
                    objectHovered.material[ 0 ].color.a = 0.5;
                    objectHovered.material[ 0 ].color.updateStyleString();

                    return;

                }

            } else {

                position = new THREE.Vector3().add( intersect.point, intersect.object.matrixRotation.transform( intersect.face.normal.clone() ) );

                brush.position.x = Math.floor( position.x / 50 ) * 50 + 25;
                brush.position.y = Math.floor( position.y / 50 ) * 50 + 25;
                brush.position.z = Math.floor( position.z / 50 ) * 50 + 25;

                return;

            }

        }

    }

    brush.position.y = 2000;

}

function render() {

    renderer.render( scene, camera );

}

function save() {

    linesMaterial.color.setRGBA( 0, 0, 0, 0 );
    brush.position.y = 2000;
    render();

    //window.open( renderer.domElement.toDataURL('image/png'), 'mywindow' );    // saves as png
    saveDCVox()

    linesMaterial.color.setRGBA( 0, 0, 0, 0.2 );
    render();

}

function saveDCVox() {

    var objs = scene.objects,
        objs_len = objs.length,
        obj,
        tmp,
        voxels=[],
        vox,
        num_vox,
        dim,
        j_data,
        low,
        high,
        n,
        meshes = 0,
        i, j;

    for (i=0; i<objs_len; i++) {
        obj = objs[i];
        if (!(obj instanceof THREE.Mesh)) continue;
        tmp = convertColorObjToRGBA(obj);
        if (tmp === undefined) {
            continue;
        } else {
            meshes += 1;
        }
        if (meshes <= 1) {
            continue
        }
        tmp = convertPosObjToIndexArr(obj).concat(tmp);
        for (j=0; j<3; j++) {
            n = parseInt(tmp[j], 10);
            if (low === undefined) {
                low = n;
            }
            if (high === undefined) {
                high = n;
            }
            if (n < low) {
                low = n;
            } else if (n > high) {
                high = n;
            }
        }
        voxels.push(tmp);
    }
    
    low = -low;
    high += low;
    if (low) {
        num_vox = voxels.length;
        for (i=0; i<num_vox; i++) {
            vox = voxels[i];
            for (j=0; j<3; j++) {
                vox[j] += low;
            }
        }
    }
    for (i=0; i<num_vox; i++) {
        vox = voxels[i];
        for (j=0; j<7; j++) {
            vox[j] = parseInt(vox[j], 10);
        }
    }

    j_data = {
        'voxels'    :   voxels,
        'vosize'    :   0.2,
        'dim'       :   [high, high, high]
    }
    j_data = JSON.stringify(j_data);
    //window.open(j_data, 'mywindow');    // save as json
    displayJSONdata(j_data);
}

function displayJSONdata(j_data) {
    // create big textarea,
    // display it on screen
    // highlight text
    var txt = $('<textarea></textarea>'),
        input = $('<input />'),
        b = $('body');
    txt.attr({
        'rows': 50,
        'cols': 150
    });
    txt.css({
        //'position': 'absolute'
    });

    input.val('Close');
    input.attr('type', 'submit');
    input.click(function (e) {
        var i = $(this);
        txt.remove();
        i.remove();
        b.children().show();
    });
    
    b.children().hide();
    b.append(txt);
    txt.val(j_data).focus().select();
    b.append(input);

}

function convertColorObjToRGBA(obj) {
    var m = obj.material,
        mlen = m.length,
        i,
        mat,
        c,
        rgba;
    for (i=0; i<mlen; i++) {
        mat = m[i];
        if (!(mat instanceof THREE.MeshColorFillMaterial)) continue;
        c = mat.color;
        rgba = [c.r*255, c.g*255, c.b*255, c.a*255];
    }
    return rgba;
}
    

function convertPosObjToIndexArr(obj) {
    var n = [];
    n.push(convertPosToIndex(obj.position.x));
    n.push(convertPosToIndex(obj.position.y));
    n.push(convertPosToIndex(obj.position.z));
    return n;
}

function convertPosToIndex(pos) {
    var i = (pos - 25)/50;
    return i;
}

function clear() {

    if ( !confirm( 'Are you sure?' ) ) {

        return

    }

    window.location.hash = "";

    var i = 0;

    while ( i < scene.objects.length ) {

        object = scene.objects[ i ];

        if ( object instanceof THREE.Mesh && object !== plane && object !== brush ) {

            scene.removeObject( object );
            continue;
        }

        i ++;
    }

    updateHash();
    render();

}

// https://gist.github.com/665235

function decode( string ) {

    var output = [];
    string.split('').forEach( function ( v ) { output.push( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".indexOf( v ) ); } );
    return output;

}

function encode( array ) {

    var output = "";
    array.forEach( function ( v ) { output += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".charAt( v ); } );
    return output;

}
