Template.eventsList.helpers({

	events: function() {
	    return Events.find({type:"Nest"},{sort:{updated_at:-1},limit:1}).fetch();
	}
})
