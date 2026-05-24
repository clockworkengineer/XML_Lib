pipeline {
    agent any
    stages {
        stage ('Build') {
            steps {
                sh 'echo "Building..."'
                sh 'chmod +x ./scripts/Linux-Build.sh'
                sh './scripts/Linux-Build.sh'
                archiveArtifacts artifacts: 'Release/tests/XML_Lib_Unit_Tests, Release/libXML_Lib.a, Release/examples/XML_*', fingerprint: true
            }
        }
        stage ('Test'){
            steps {
                sh 'echo "Testing..."'
                sh 'chmod +x ./scripts/Linux-Run-Tests.sh'
                sh './scripts/Linux-Run-Tests.sh'
            }
        }
        stage ('Performance'){
            steps {
                sh 'echo "Running performance validation..."'
                sh 'chmod +x ./scripts/Linux-Run-Performance.sh'
                sh './scripts/Linux-Run-Performance.sh'
            }
        }
        stage ('Compliance'){
            steps {
                sh 'echo "Running compliance validation..."'
                sh 'chmod +x ./scripts/Linux-Run-Compliance.sh'
                sh './scripts/Linux-Run-Compliance.sh'
            }
        }
    }
}