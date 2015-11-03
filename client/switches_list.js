Template.switch1List.helpers({

	switch1: function() {
	    return Events.find({type:"Switch",switch_id:"2"},{sort:{updated_at:-1},limit:1}).fetch();
	}
})

Template.switch2List.helpers({

	switch2: function() {
	    return Events.find({type:"Switch",switch_id:"3"},{sort:{updated_at:-1},limit:1}).fetch();
	}
})







