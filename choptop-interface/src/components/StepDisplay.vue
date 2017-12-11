<template>
	<div class="stepWrapper">
		<div class="middleRow">
			<div :class="{arr:true, leftarr:true, noshow: stepIdx == 0}"><icon name="angle-left" scale="4"></icon></div>
			<div class="step">
					Step:{{stepIdx+1}}/{{steps.length}}<br/>
					{{steps[stepIdx].text}}
			</div>
			<div :class="{rightArr:true, arr:true, noshow: stepIdx == steps.length - 1}"><icon name="angle-right" scale="4"></icon></div>
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

	.middleRow{
		display:flex;
		flex-direction: row;
	  	justify-content: space-between;
		padding:10px;
	}

	.arr{
		font-size: 100px;
		opacity: 1;
		transition: 0.5s;
	}

	.arr.noshow{
		opacity:0;
		transition: 0.5s;
	}
	
</style>