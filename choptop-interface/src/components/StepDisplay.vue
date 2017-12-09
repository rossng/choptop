<template>
	<div>
		<div class="step">
			<!-- The time in the Json file is in minutes, didn't want to add another component for that -->
				Step:{{stepIdx+1}}/{{steps.length}}
				{{steps[stepIdx].text}}
		</div>
		
		{{getTime(stepIdx)}}
		<div v-if = "timeDisp">
			Display the time:
			<div class="timer">
				{{steps[stepIdx].time}}
			</div>
		</div>
	</div>


	
</template>

<script>


	export default {
		name: 'stepDisplay',
		components: {
		},

		props: ['steps', 'stepIdx','time'],
		data () {
			return {
				selected: true,
				timeDisp:false,
			  
			}
		},

		methods: {
		  	getTime: function(index){

		  		if(this.steps[index].time != 0){
		  			this.timeDisp = true;
		  		}
		  	}
		}
	}
</script>



<style scoped>
	.step{
		font-style: italic;
	  	font-size: 30px;
	  	margin:30px;
	}
	
</style>