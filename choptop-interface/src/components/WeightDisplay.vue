<template>
	<div class="weightDisplay">
		Weight:
		{{currentWeight - tareValue}}
	</div>
</template>

<script>
	export default {
	  name: 'weightDisplay',
	  props:['eventBus'],
	  methods:{

	  },
	  created (){
	  	this.eventBus.$on("weight", this.handleNewWeight);
	  	this.eventBus.$on("tare", this.handleTare);
	  },
	  methods:{
	  	handleNewWeight(weight){
	  		console.log("new weight" + weight)
	  		this.currentWeight = weight;
	  	},
	  	handleTare(){
	  		this.tareValue = this.weight;
	  	}
	  },
	  data () {
	    return {
	      currentWeight:0,
	      tareValue:0,
	    }
	  }
	}
</script>



<style scoped>

	
</style>
