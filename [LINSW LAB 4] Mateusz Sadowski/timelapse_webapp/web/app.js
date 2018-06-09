$(function () {
    refresh();
})

function refresh() {
    fetch("/refresh").then(data => data.text()).then((data) => {
        let config = data.split(" ");
        if (config[0] == "1") {
            $("#radiobox_on").prop("checked", true);
            $("#radiobox_off").prop("checked", false);

        }
        else {
            $("#radiobox_on").prop("checked", false);
            $("#radiobox_off").prop("checked", true);
        }
        $("#control_item_text_input").val(config[1]);
    })
}