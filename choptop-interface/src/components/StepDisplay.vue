<template>
	<div>
		<div class="step">
			<!-- The time in the Json file is in minutes, didn't want to add another component for that -->
				Step:{{stepIdx+1}}/{{steps.length}}
				{{steps[stepIdx].text}}
		</div>
		
		<div v-if = "hasTime()">
			Press DOWN to start timer
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
		  	hasTime(){
		  		return this.steps[this.stepIdx].time != undefined
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