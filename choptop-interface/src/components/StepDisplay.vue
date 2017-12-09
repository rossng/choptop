<template>
	<div class="step">
			Step:{{stepIdx+1}}/{{steps.length}}
			{{steps[stepIdx].text}}

	</div>
</template>

<script>


	export default {
		name: 'stepDisplay',
		components: {
		},

		props: ['steps', 'stepIdx'],
		data () {
			return {
				selected: true
			  
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