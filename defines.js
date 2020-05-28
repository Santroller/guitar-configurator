.pragma library
.import net.tangentmc 1.0 as TangentMC

function fillCombobox(name) {
    var model = [];
    var entries = TangentMC.ArdwiinoDefines[`get_${name}_entries`]();
    for (let key in entries) {
        model.push({key: key, value: entries[key]});
    }
    model.sort((s,s2)=>s.value-s2.value);
    return model;
}

//Turn a type id into the keys for each box
function findTypeDevice(id) {
    var st = getTypeCombobox();
    for (var type in st) {
        var t = st[type];
        var key = t.find(v => v.value === id);
        if (key) {
            return [type,key.key];
        }
    }
}

function getTypeCombobox() {
    var st = fillCombobox("SubType");
    var keys = [...new Set(Object.values(st).map(k=>k.key.split(" ")[0]))];
    var values = st.reduce((obj,value)=>{
                               var key = value.key.split(" ")[0];
                               value.key = value.key.split(" ").splice(1).join(" ");
                               var arr = [];
                               if (obj.hasOwnProperty(key)) {
                                   arr = obj[key];
                               }
                               arr.push(value);
                               obj[key]=arr;
                               return obj;
                           },{});
    return values;
}

function getBoardImage(type) {
    return getBoardBase(type)+"/Base.svg";
}

function getBoardComponents(type) {
    return getBoardBase(type)+"/components/";
}

function getBoardBase(type) {
    let a = findTypeDevice(type);
    //The controller type needs to be joined up as the images are not stored with spaces.
    a[1] = a[1].split(" ").join("");
    if (!a[1].length) a = [a[0]];
    return "images/"+a.join("/");
}
