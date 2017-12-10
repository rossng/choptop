<template>
	<div class="weightDisplay">
		Weight:
		<AnimatedInt :value="displayWeight()" />
	</div>
</template>

<script>
	import AnimatedInt from './AnimatedInteger'
	export default {
	  name: 'weightDisplay',
	  props:['eventBus'],
	  components:{
	  	AnimatedInt
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
	  	},
	  	displayWeight(){
	  		return this.round(this.currentWeight - this.tareValue);
	  	},
	  	round(weight){
	  		// Rounds the weight to the closest 5 grams. Only rounds the displayed value so the error wouldn't build up.
		   	return Math.round(weight/5)*5;
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
	.weightDisplay{
		position: fixed;
		bottom: 45px;
		text-align: center;
		width: 800px;
		left: 0px;
	}
	
</style>
